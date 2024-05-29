#include <iostream>
#include <fstream>
#include <filesystem>
#include "file.h"
file::paths paths;
namespace fs = std::filesystem;

void gen_example_appconf()
{
    std::ofstream outfile("rename_to_APPCONF");

    outfile << R"({
    "appname":"name of an app",
    "appversion": "1.0.0",
    "appsrc": "local",
    "pkgarchivetype": "tar.gz",
    "pmupdatecmd": "apt update -y",
    "pminstallcmd": "apt install -y",
    "deps": [
        "nano",
        "mc"
    ],
    "prepcmds": [
        "echo Commands that will be executed before installation"
    ],
    "cmds": [
        "echo Commands that will be executed after installation"
    ],
    "APPCONFVERSION": 1
}
)" << std::endl;

    outfile.close();
}

void display_installed_packages()
{
    for (const auto &entry : fs::directory_iterator(paths.local_repo_directory))
        std::cout << entry.path() << std::endl;
}