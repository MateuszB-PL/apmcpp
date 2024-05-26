#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include "pkg.cpp"
#include "mkfile.cpp"
//main

class programInfo {
    public:
        std::string version = "1.0.0";
        std::string cppver;
        std::string chkCppVer(){
            if (__cplusplus == 202101L) cppver = "C++23";
            else if (__cplusplus == 202002L) cppver = "C++20";
            else if (__cplusplus == 201703L) cppver = "C++17";
            else if (__cplusplus == 201402L) cppver = "C++14";
            else if (__cplusplus == 201103L) cppver = "C++11";
            else if (__cplusplus == 199711L) cppver = "C++98";
            else cppver = "ERROR: version not in db";
            return cppver;
        }
};

int main(int argc, char *argv[]) {
    programInfo programInfo;
    if (getuid() != 0){ std::cout<< vars.prefix << "Please run as root"; exit(1);}
    for(;;)
    {
        switch(getopt(argc, argv, "iu:s:hev")) // note the colon (:) to indicate that 'n' has a parameter and is not a switch
        {
        case 'i':
            std::cout << vars.prefix <<"Installing from APPCONF"<<std::endl;
            install();
            continue;

        case 'u':
            uninstall(optarg);
            continue;
        case 's':
            continue;
        case 'e':
            gen_example_appconf();
            continue;
        case 'v':
            std::cout<< "C++ build version: "<< __cplusplus << " or "<< programInfo.chkCppVer() <<" | APM - App Package Manager version: "<< programInfo.version <<std::endl;
            continue;
        case '?':
        case 'h':
        default :
          printf("Help N/A\n");
            break;

        case -1:
            break;
        }

        break;
    }
    
}