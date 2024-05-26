#include <iostream>
#include <filesystem>
#include "libarchive.cpp"
#include "vars.cpp"
namespace fs = std::filesystem;
vars v;

void create_recursive_symlink(const fs::path& target, const fs::path& link) {
    if (fs::is_directory(target)) {
        fs::create_directory_symlink(target, link);
        for (const auto& entry : fs::directory_iterator(target)) {
            create_recursive_symlink(entry.path(), link / entry.path().filename());
        }
    } else if (fs::is_regular_file(target)) {
        fs::create_symlink(target, link);
    }
}


void sync(const std::string& jsonarray){
    for (std::string value : v.j[jsonarray]) {
        std::string cmd = v.pminstallcmd + " " + std::string(value);
        std::cout << cmd.c_str() << std::endl;
        system(cmd.c_str());
    }
}
void forexec(const std::string& jsonarray){
    for (std::string value : v.j[jsonarray]) {
        system(value.c_str());
    }
}
void install()
{
    
    std::cout<< "Apps that will be installed: " << v.appname << std::endl <<std::endl;
    std::cout<< "Dependencies that will be installed: ";
    for (std::string value : v.j["deps"]) {
            std::string cmd = std::string(value) + " ";
            std::cout << cmd.c_str();
    }
    // std::cout<< "Commands that will be executed: ";
    // for (std::string value : j["cmds"]) {
    //     std::string cmd = std::string(value) + " ";
    //     std::cout << cmd.c_str();
    // }
    std::cout << std::endl << std::endl;
    std::cout<< "Continue? [y/N]: ";

    char userinput = std::cin.get();
    if (tolower(userinput) == 'y'){
        std::cout << v.prefix << "Executing preparation cmds... "<< std::endl;
        forexec("prepcmds");
        std::cout << v.prefix << "Updating repositories..." << std::endl;
        system(v.pmupdatecmd.c_str());
        std::cout << v.prefix << "Repositories update done" << std::endl;
        std::cout << v.prefix << "Installing dependencies..." << std::endl;
        sync("deps");
        std::cout << v.prefix << "Installing dependencies done" << std::endl;
        std::cout << v.prefix << "Please wait, extracting in progress, this might take a while..." << std::endl;
        std::string extractstr = v.appname + "." + std::string(v.pkgarchivetype);
        extract(extractstr.c_str());
        const auto copyOptions = std::filesystem::copy_options::update_existing
                           | std::filesystem::copy_options::recursive
                           ;

        std::filesystem::copy(v.appname, "/usr/apps/" + v.appname, copyOptions);
        std::filesystem::remove_all(v.appname);
        std::cout << v.prefix << "Extracting and Copying done" << std::endl;
        
        try
        {
            if (fs::exists(v.root) || fs::is_directory(v.root)) {
                for (const auto& entry : fs::recursive_directory_iterator(v.root)) {
                    fs::path target = entry.path();
                    fs::path link = v.link_base / fs::relative(target, v.root);
                    create_recursive_symlink(target, link);
                }
                std::cout << v.prefix << "System root files linked to /.\n" << std::endl;
            }
            if (fs::exists(v.desktop) || fs::is_directory(v.desktop)) {
                for (const auto& entry : fs::recursive_directory_iterator(v.desktop)) {
                    fs::path target = entry.path();
                    fs::path link = v.applicationdesktop / fs::relative(target, v.desktop);
                    create_recursive_symlink(target, link);
                }
                std::cout << v.prefix << "Desktop files linked to /usr/share/applications.\n" << std::endl;
            }
        }
        catch (std::exception e)
        {
            std::cout << v.prefix << "Already linked, (skipping)"<<std::endl;
        }
        forexec("cmds");

        std::cout << v.prefix <<"Installation Complete!"<<std::endl;
    }
}
void uninstall(std::string appname){
    std::string apppath = "/usr/apps/" + appname;
    if (fs::exists(apppath) || fs::is_directory(apppath)) {
        fs::remove_all(apppath);
        std::cout << v.prefix << "App removed from local APM repo: " << appname << std::endl;
    } else {
        std::cout << v.prefix << "App package couldn't be found in local APM repo: " << appname <<std::endl;
    }
}