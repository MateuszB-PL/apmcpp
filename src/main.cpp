#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

#include "file.h"
#include "pkg.h"

pkg::jvars jvars;
pkg::vars vars;
file::paths paths;

#include "libarchive.cpp"
#include "filemgr.cpp"
#include "pkg.cpp"
// main

class programInfo
{
public:
    std::string version = "alpha0.2";
    std::string cppver;
    std::string chkCppVer()
    {
        if (__cplusplus == 202101L)
            cppver = "C++23";
        else if (__cplusplus == 202002L)
            cppver = "C++20";
        else if (__cplusplus == 201703L)
            cppver = "C++17";
        else if (__cplusplus == 201402L)
            cppver = "C++14";
        else if (__cplusplus == 201103L)
            cppver = "C++11";
        else if (__cplusplus == 199711L)
            cppver = "C++98";
        else
            cppver = "ERROR: version not in db";
        return cppver;
    }
};
void checkroot()
{
    if (getuid() != 0)
    {
        std::cout << vars.prefix << "Please run as root";
        exit(1);
    }
}
int main(int argc, char *argv[])
{
    programInfo programInfo;
    for (;;)
    {
        switch (getopt(argc, argv, "iu:shevlIB")) // note the colon (:) to indicate that 'n' has a parameter and is not a switch
        {
        case 'i':
            checkroot(); 
            std::cout << vars.prefix << "Installing from APPCONF" << std::endl;
            install();
            break;

        case 'u':
            checkroot();
            uninstall(optarg);
            break;
        case 's':
            break;
        case 'e':
            gen_example_appconf();
            break;
        case 'v':
            std::cout << "C++ build version: " << __cplusplus << " or " << programInfo.chkCppVer() << "\n APM - App Package Manager version: " << programInfo.version <<std::endl;
            break;
        case 'l':
            display_installed_packages();
            break;
        case 'I':
            checkroot();
            fs::rename(argv[0], "apm");
            fs::copy("apm", "/usr/bin", fs::copy_options::update_existing);
            fs::create_directories(paths.local_repo_directory);
            std::cout<< "Succesfully installed/updated App Package Manager!" << std::endl; 
            break;
        case 'B':
            system("g++ main.cpp -o main -larchive");
            std::cout << "Build finished!" <<std::endl;
            break;
        case '?':
        case 'h':
        default:
            std::cout << R"( 
    sudo apm -i - install from APPCONF
    sudo apm -u <package name> - uninstall package
    apm -e - generate example appconf
    apm -l - list all installed apps
    apm -v - displays c++ compilation version and APM version
 )";
            break;

        case -1:
            break;
        }

        break;
    }
}