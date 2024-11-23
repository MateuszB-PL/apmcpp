#include <archive.h>
#include <archive_entry.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;

#include "pkg.h"
#include "sync.cpp"

pkg::application_info application_info;

#include "filemgr.cpp"
#include "libarchive.cpp"
#include "pkg.cpp"
// main

class programInfo {
 public:
  std::string version = "prebeta0.5";
  std::string cppver;
  std::string chkCppVer() {
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
      cppver = "ERROR: version not found!";
    return cppver;
  }
};
std::string getProcessorArchitecture() {
#if defined(__aarch64__) || defined(_M_ARM64)
  return "arm64";
#elif defined(__x86_64__) || defined(_M_X64)
  return "x64_86";
#elif defined(__i386__) || defined(_M_IX86)
  return "x86";
#else
  return "unknown";
#endif
}
void checkroot() {
  if (getuid() != 0) {
    std::cout << pkg::constant_variables::prefix << "Please run as root";
    exit(1);
  }
}
int main(int argc, char *argv[]) {
  programInfo program_information;
  std::string working_directory = fs::current_path();
  for (;;) {
    switch (getopt(argc, argv, "iu:s:hevlIBr"))  // note the colon (:) to indicate that 'n'
    {
      case 'i':
        checkroot();
        std::cout << pkg::constant_variables::prefix << "Installing from APPCONF" << std::endl;
        install();
        break;

      case 'u':
        checkroot();
        uninstall(optarg);
        break;
      case 's':
        checkroot();
        fs::create_directories("/tmp/apppackagemanager/" + std::string(optarg));
        fs::current_path("/tmp/apppackagemanager/" + std::string(optarg));
        syncrepo("https://raw.githubusercontent.com/MateuszB-PL/apr/" + getProcessorArchitecture() + "/" + getProcessorArchitecture() + "/pkg/" + std::string(optarg) + "/APPCONF", "APPCONF");
        std::cout << "Succesfully synced APPCONF" << std::endl;
        install();
        std::cout << "Succesfully installed " << std::string(optarg) << std::endl;
        fs::current_path(working_directory);
        fs::remove_all("/tmp/apppackagemanager");

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
APM HELP:

sudo apm -i - install from APPCONF
sudo apm -s - sync and install package from repo
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
