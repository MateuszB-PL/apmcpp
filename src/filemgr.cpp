void generate_example_appconf()
{
    std::ofstream outfile("rename_to_APPCONF");

    outfile << R"({
    "appname":"example",
    "appversion": "1.0.0",
    "pkgarchivetype": "tar.gz",
    "pmupdatecmd": "dnf update -y",
    "pminstallcmd": "dnf install -y",
    "deps": [
        "nano",
        "mc"
    ],
    "prep": [
        "echo Commands that will be executed before installation"
    ],
    "post": [
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