#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <unistd.h>
#include <archive.h>
#include <archive_entry.h>
#include <string>
#include <filesystem>

using json = nlohmann::json;

std::ifstream f("APPCONF");
json j = json::parse(f);
std::string appname = j["appname"];
std::string pminstallcmd = j["pminstallcmd"];
std::string pkgarchivetype = j["pkgarchivetype"];
std::string pmupdatecmd = j["pmupdatecmd"];
std::string prefix = "[APM] ";

namespace fs = std::filesystem;

void create_recursive_symlink(const fs::path& target, const fs::path& link) {
    if (fs::is_directory(target)) {
        // Utwórz link do katalogu docelowego
        fs::create_directory_symlink(target, link);

        // Przejdź przez wszystkie elementy w katalogu i utwórz symlinki rekursywnie
        for (const auto& entry : fs::directory_iterator(target)) {
            create_recursive_symlink(entry.path(), link / entry.path().filename());
        }
    } else if (fs::is_regular_file(target)) {
        // Utwórz link do pliku docelowego
        fs::create_symlink(target, link);
    }
}
static int
copy_data(struct archive *ar, struct archive *aw)
{
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (r);
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            fprintf(stderr, "%s\n", archive_error_string(aw));
            return (r);
        }
    }
}

static void
extract(const char *filename)
{
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    if ((r = archive_read_open_filename(a, filename, 10240)))
        exit(1);
    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN)
            exit(1);
        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        else if (archive_entry_size(entry) > 0) {
            r = copy_data(a, ext);
            if (r < ARCHIVE_OK)
                fprintf(stderr, "%s\n", archive_error_string(ext));
            if (r < ARCHIVE_WARN)
                exit(1);
        }
        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            fprintf(stderr, "%s\n", archive_error_string(ext));
        if (r < ARCHIVE_WARN)
            exit(1);
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
}

void sync(const std::string& jsonarray){
    for (std::string value : j[jsonarray]) {
        std::string cmd = pminstallcmd + " " + std::string(value);
        std::cout << cmd.c_str() << std::endl;
        system(cmd.c_str());
    }
}
void forexec(const std::string& jsonarray){
    for (std::string value : j[jsonarray]) {
        std::cout << jsonarray.c_str() << std::endl;
        system(jsonarray.c_str());
    }
}
void install()
{
    std::cout<< "Apps that will be installed: " << appname << std::endl <<std::endl;
    std::cout<< "Dependencies that will be installed: ";
    for (std::string value : j["deps"]) {
            std::string cmd = std::string(value) + " ";
            std::cout << cmd.c_str();
    }
    std::cout<< "Commands that will be executed: ";
    for (std::string value : j["cmds"]) {
        std::string cmd = std::string(value) + " ";
        std::cout << cmd.c_str();
    }
    std::cout << std::endl << std::endl;
    std::cout<< "Continue? [y/N]: ";

    char userinput = std::cin.get();
    if (tolower(userinput) == 'y'){
        std::cout << prefix << "Updating repositories..." << std::endl;
        system(pmupdatecmd.c_str());
        std::cout << prefix << "Repositories update done" << std::endl;
        std::cout << prefix << "Installing dependencies..." << std::endl;
        sync("deps");
        std::cout << prefix << "Installing dependencies done" << std::endl;
        std::cout << prefix << "Please wait, extracting in progress, this might take a while..." << std::endl;
        std::string extractstr = appname + "." + std::string(pkgarchivetype);
        extract(extractstr.c_str());
        const auto copyOptions = std::filesystem::copy_options::update_existing
                           | std::filesystem::copy_options::recursive
                           ;

        std::filesystem::copy(appname, "/usr/apps/" + appname, copyOptions);
        std::filesystem::remove_all(appname);
        std::cout << prefix << "Extracting and Copying done" << std::endl;
        fs::path root = "/usr/apps/"+ appname +"/root";
        fs::path desktop = "/usr/apps/"+appname+"/desktop";
        fs::path link_base = "/";
        fs::path applicationdesktop = "/usr/share/applications";
        try
        {
            if (fs::exists(root) || fs::is_directory(root)) {
                for (const auto& entry : fs::recursive_directory_iterator(root)) {
                    fs::path target = entry.path();
                    fs::path link = link_base / fs::relative(target, root);
                    create_recursive_symlink(target, link);
                }
                std::cout << prefix << "System root files linked to /.\n" << std::endl;
            }
            if (fs::exists(desktop) || fs::is_directory(desktop)) {
                for (const auto& entry : fs::recursive_directory_iterator(desktop)) {
                    fs::path target = entry.path();
                    fs::path link = applicationdesktop / fs::relative(target, desktop);
                    create_recursive_symlink(target, link);
                }
                std::cout << prefix << "Desktop files linked to /usr/share/applications.\n" << std::endl;
            }
        }
        catch (std::exception e)
        {
            std::cout << prefix << "Already linked, (skipping)"<<std::endl;
        }
        std::cout << prefix <<"Installation Complete!"<<std::endl;
    }
}
int main(int argc, char *argv[]) {
    if (getuid() != 0){ std::cout<< "Please run as root"; exit(1);}
    for(;;)
    {
        switch(getopt(argc, argv, "iu:h")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
        case 'i':
            std::cout<<"INSTALLING PACKAGE"<<std::endl;
            install();
            continue;

        case 'u':
            printf("parameter 'b' specified with the value %s\n", optarg);
            continue;

        case '?':
        case 'h':
        default :
          printf("Help/Usage Example\n");
            break;

        case -1:
            break;
        }

        break;
    }
    
}