#include <iostream>
#include <fstream>

void gen_example_appconf()
{
    std::ofstream outfile("rename_to_APPCONF");

    outfile << R"({
    "appname":"name of an app",
    "appversion": "1.0.0",
    "appsrc": "local",
    "pkgarchivetype": "tar.gz",
    "pmupdatecmd": "ex. apt update -y",
    "pminstallcmd": "ex. apt install -y",
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