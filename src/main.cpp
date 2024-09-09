#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;

#include "file.h"
#include "pkg.h"
#include "sync.cpp"

pkg::application_info application_info;
pkg::constant_variables constant_variables;
file::paths paths;

#include "libarchive.cpp"
#include "filemgr.cpp"
#include "pkg.cpp"
// main

class programInfo
{
public:
    std::string version = "prebeta0.4";
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
        std::cout << constant_variables.prefix << "Please run as root";
        exit(1);
    }
}
int main(int argc, char *argv[])
{
    programInfo program_information;
    std::string working_directory = fs::current_path();
    for (;;)
    {
        switch (getopt(argc, argv, "iu:s:hevlIBr")) // note the colon (:) to indicate that 'n' has a parameter and is not a switch
        {
        case 'i':
            checkroot();
            std::cout << constant_variables.prefix << "Installing from APPCONF" << std::endl;
            install();
            break;

        case 'u':
            checkroot();
            uninstall(optarg);
            break;
        case 's':
            fs::create_directory("/tmp" + std::string(optarg));
            fs::current_path("/tmp" + std::string(optarg));
            syncrepo("https://raw.githubusercontent.com/MateuszB-PL/apr/x64_86/x64_86/pkg/" + std::string(optarg) + "/APPCONF", "APPCONF");
            std::cout << "Succesfully synced APPCONF" << fs::current_path() << std::endl;
            install();
            std::cout << "Succesfully installed " << std::string(optarg) << std::endl;
            fs::current_path(working_directory);
            fs::remove_all("/tmp" + std::string(optarg));
            break;
        case 'e':
            generate_example_appconf();
            break;
        case 'v':
            std::cout << "C++ build version: " << __cplusplus << " or " << program_information.chkCppVer() << "\n APM - App Package Manager version: " << program_information.version << std::endl;
            break;
        case 'l':
            display_installed_packages();
            break;
        case -1:
        case '?':
        case 'h':
        default:
            std::cout << R"(
                sudo apm -i - install from APPCONF
                sudo apm -s - sync package from repo
                sudo apm -u <package name> - uninstall package
                apm -e - generate example appconf
                apm -l - list all installed apps
                apm -v - displays c++ compilation version and APM version
            )";
            break;
        }

        break;
    }
}