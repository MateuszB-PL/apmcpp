// pkg.cpp
#include <iostream>
#include <filesystem>
#include "libarchive.cpp"
#include "pkg.h"
namespace fs = std::filesystem;



pkg::jvars jvars;
pkg::vars vars;
pkg::mgr mgr;
std::string link_base = "/";


// pkg
void create_recursive_symlink(const fs::path &target, const fs::path &link)
{
    if (fs::is_directory(target))
    {
        fs::create_directory_symlink(target, link);
        for (const auto &entry : fs::directory_iterator(target))
        {
            create_recursive_symlink(entry.path(), link / entry.path().filename());
        }
    }
    else if (fs::is_regular_file(target))
    {
        fs::create_symlink(target, link);
    }
}

void sync(const std::string &jsonarray)
{
    jvars.pminstallcmd = jvars.j["pminstallcmd"];
    for (std::string value : jvars.j[jsonarray])
    {
        std::string cmd = jvars.pminstallcmd + " " + std::string(value);
        std::cout << cmd.c_str() << std::endl;
        system(cmd.c_str());
    }
}
void forexec(const std::string &jsonarray)
{
    for (std::string value : jvars.j[jsonarray])
    {
        system(value.c_str());
    }
}
void install()
{
    jvars.pkgarchivetype = jvars.j["pkgarchivetype"];
    jvars.pmupdatecmd = jvars.j["pmupdatecmd"];
    jvars.appname = jvars.j["appname"];
    vars.root = "/usr/apps/" + jvars.appname + "/root";
    vars.desktop = "/usr/apps/" + jvars.appname + "/desktop";
    vars.applicationdesktop = "/usr/share/applications";
    std::cout << "Apps that will be installed: " << jvars.appname << std::endl
              << std::endl;
    std::cout << "Dependencies that will be installed: ";
    for (std::string value : jvars.j["deps"])
    {
        std::string cmd = std::string(value) + " ";
        std::cout << cmd.c_str();
    }
    // std::cout<< "Commands that will be executed: ";
    // for (std::string value : j["cmds"]) {
    //     std::string cmd = std::string(value) + " ";
    //     std::cout << cmd.c_str();
    // }
    std::cout << std::endl
              << std::endl;
    std::cout << "Continue? [y/N]: ";

    char userinput = std::cin.get();
    if (tolower(userinput) == 'y')
    {
        std::cout << vars.prefix << "Executing preparation cmds... " << std::endl;
        forexec("prepcmds");
        std::cout << vars.prefix << "Updating repositories..." << std::endl;
        system(jvars.pmupdatecmd.c_str());
        std::cout << vars.prefix << "Repositories update done" << std::endl;
        std::cout << vars.prefix << "Installing dependencies..." << std::endl;
        sync("deps");
        std::cout << vars.prefix << "Installing dependencies done" << std::endl;
        std::cout << vars.prefix << "Please wait, extracting in progress, this might take a while..." << std::endl;
        std::string extractstr = jvars.appname + "." + std::string(jvars.pkgarchivetype);
        extract(extractstr.c_str());
        const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;

        std::filesystem::copy(jvars.appname, "/usr/apps/" + jvars.appname, copyOptions);
        std::filesystem::remove_all(jvars.appname);
        std::cout << vars.prefix << "Extracting and Copying done" << std::endl;

        try
        {
            std::cout << vars.root;
            if (fs::exists(vars.root) || fs::is_directory(vars.root))
            {
                for (const auto &entry : fs::recursive_directory_iterator(vars.root))
                {
                    fs::path target = entry.path();
                    fs::path link = link_base / fs::relative(target, vars.root);
                    create_recursive_symlink(target, link);
                }
                std::cout << vars.prefix << "System root files linked to /.\n"
                          << std::endl;
            }
            std::cout << vars.desktop;
            if (fs::exists(vars.desktop) || fs::is_directory(vars.desktop))
            {
                for (const auto &entry : fs::recursive_directory_iterator(vars.desktop))
                {
                    fs::path target = entry.path();
                    fs::path link = vars.applicationdesktop / fs::relative(target, vars.desktop);
                    create_recursive_symlink(target, link);
                }
                std::cout << vars.prefix << "Desktop files linked to /usr/share/applications.\n"
                          << std::endl;
            }
        }
        catch (std::exception e)
        {
            std::cout << vars.prefix << "Already linked, (skipping)" << std::endl;
        }
        forexec("cmds");

        std::cout << vars.prefix << "Installation Complete!" << std::endl;
    }
}
void uninstall(std::string appname)
{
    vars.apppath = "/usr/apps/" + appname;
    if (fs::exists(vars.apppath) || fs::is_directory(vars.apppath))
    {
        fs::remove_all(vars.apppath);
        std::cout << vars.prefix << "App removed from local APM repo: " << appname << std::endl;
    }
    else
    {
        std::cout << vars.prefix << "App package couldn't be found in local APM repo: " << appname << std::endl;
    }
}