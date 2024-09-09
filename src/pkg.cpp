// pkg.cpp

// pkg
void create_recursive_symlink(const fs::path &target, const fs::path &link)
{
    try
    {
        if (!fs::exists(link))
        {
            fs::create_symlink(target, link);
        }
    }
    catch (const fs::filesystem_error &e)
    {
        std::cerr << "Error creating symlink: " << e.what() << std::endl;
    }
}

void sync(const std::string &jsonarray)
{
    try
    {
        for (std::string value : application_info.j[jsonarray])
        {
            std::string cmd = application_info.pminstallcmd + " " + std::string(value);
            std::cout << cmd.c_str() << std::endl;
            system(cmd.c_str());
        }
    }
    catch (std::exception e)
    {
        std::cout << "No dependencies." << std::endl;
    }
}
void forexec(const std::string &jsonarray)
{
    for (std::string value : application_info.j[jsonarray])
    {
        system(value.c_str());
    }
}

std::string replacePathPrefix(const std::string &originalPath, const std::string &oldPrefix, const std::string &newPrefix)
{
    if (originalPath.find(oldPrefix) == 0)
    {
        std::string newPath = originalPath;
        newPath.replace(0, oldPrefix.length(), newPrefix);
        return newPath;
    }
    else
    {
        return originalPath;
    }
}

void indexFiles(const std::string &indexFile, const std::string &sourceDir, const std::string &replacementDir)
{
    std::ofstream outFile(indexFile, std::ios::app);
    if (!outFile.is_open())
    {
        std::cerr << "Cannot open INDEXFILE" << indexFile << std::endl;
        return;
    }

    try
    {
        for (const auto &entry : fs::recursive_directory_iterator(sourceDir))
        {
            if (fs::is_regular_file(entry))
            {
                std::string filePath = entry.path().string();
                std::string newFilePath = replacePathPrefix(filePath, sourceDir, replacementDir);
                outFile << newFilePath << std::endl;
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        std::cerr << "Filesystem error while indexing" << e.what() << std::endl;
    }

    outFile.close();
}
void deleteIndexedFiles(const std::string &indexFile)
{
    std::ifstream inFile(indexFile);
    if (!inFile.is_open())
    {
        std::cerr << "Cannot read FILEINDEX " << indexFile << std::endl;
        return;
    }

    std::string filePath;
    while (std::getline(inFile, filePath))
    {
        try
        {
            fs::remove(filePath);
            std::cout << "Removed: " << filePath << std::endl;
        }
        catch (const fs::filesystem_error &e)
        {
            std::cerr << "Filesystem error while removing: " << e.what() << std::endl;
        }
    }

    inFile.close();
    std::cout << "Removed all indexed files." << std::endl;
}
void install()
{
    std::ifstream f("APPCONF");
    application_info.j = json::parse(f);
    application_info.appname = application_info.j["appname"];
    application_info.pkgarchivetype = application_info.j["pkgarchivetype"];
    try
    {
        application_info.pminstallcmd = application_info.j["pminstallcmd"];
        application_info.pmupdatecmd = application_info.j["pmupdatecmd"];
    }
    catch (std::exception e)
    {
        std::cout << "Ignoring Package Manager" << std::endl;
    }

    std::cout << "Apps that will be installed: " << application_info.appname << std::endl
              << std::endl;
    std::cout << "Dependencies that will be installed: ";
    try
    {
        for (std::string value : application_info.j["deps"])
        {
            std::string cmd = std::string(value) + " ";
            std::cout << cmd.c_str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "No dependencies." << std::endl;
    }
    // std::cout<< "Commands that will be executed: ";
    // for (std::string value : j["cmds"]) {
    //     std::string cmd = std::string(value) + " ";
    //     std::cout << cmd.c_str();
    // }
    std::cout << std::endl
              << std::endl;
    std::cout << "Continue? [Y/n]: ";

    char userinput = std::cin.get();
    if (tolower(userinput) == 'n')
    {
        return;
    }
    std::cout << constant_variables.prefix << "Executing preparation commands... " << std::endl;
    forexec("prep");
    std::cout << constant_variables.prefix << "Updating repositories..." << std::endl;
    system(application_info.pmupdatecmd.c_str());
    std::cout << constant_variables.prefix << "Repositories update done" << std::endl;
    std::cout << constant_variables.prefix << "Installing dependencies..." << std::endl;
    sync("deps");
    std::cout << constant_variables.prefix << "Installing dependencies done" << std::endl;
    std::cout << constant_variables.prefix << "Please wait, extracting in progress, this might take a while..." << std::endl;
    std::string extractstr = application_info.appname + "." + std::string(application_info.pkgarchivetype);
    extract(extractstr.c_str());
    const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
    std::filesystem::copy(application_info.appname, constant_variables.app_repository_path + application_info.appname, copyOptions);
    std::filesystem::remove_all(application_info.appname);
    std::cout << constant_variables.prefix << "Extracting and Copying done" << std::endl;
    std::string FILEINDEX = constant_variables.app_repository_path + application_info.appname + "/FILEINDEX";
    std::string root_directory = constant_variables.app_repository_path + application_info.appname + "/root/";
    std::string desktop_file_directory = constant_variables.app_repository_path + application_info.appname + "/desktop/";
    indexFiles(FILEINDEX, root_directory, "/");
    indexFiles(FILEINDEX, desktop_file_directory, constant_variables.system_applications_directory);
    try
    {
        std::string root = constant_variables.app_repository_path + application_info.appname + "/root";
        std::string desktop = constant_variables.app_repository_path + application_info.appname + "/desktop";
        if (fs::exists(root) || fs::is_directory(root))
        {
            for (const auto &entry : fs::recursive_directory_iterator(root))
            {
                fs::path target = entry.path();
                fs::path link = constant_variables.link_base / fs::relative(target, root);
                create_recursive_symlink(target, link);
            }
            std::cout << constant_variables.prefix << "System root files linked to /.\n"
                      << std::endl;
        }
        if (fs::exists(desktop) || fs::is_directory(desktop))
        {
            for (const auto &entry : fs::recursive_directory_iterator(desktop))
            {
                fs::path target = entry.path();
                fs::path link = constant_variables.applicationdesktop / fs::relative(target, desktop);
                create_recursive_symlink(target, link);
            }
            std::cout << constant_variables.prefix << "Desktop files linked to:" << constant_variables.applicationdesktop
                      << std::endl;
        }
    }
    catch (std::exception e)
    {
        std::cout << constant_variables.prefix << "Already linked, (skipping)" << std::endl;
    }
    std::cout << constant_variables.prefix << "Executing post-install commands..." << std::endl;
    forexec("post");

    std::cout << constant_variables.prefix << "Installation Complete!" << std::endl;
}
void uninstall(std::string appname)
{
    std::string apppath = constant_variables.app_repository_path + appname;
    if (fs::exists(apppath) || fs::is_directory(apppath))
    {
        deleteIndexedFiles(apppath + "/FILEINDEX");
        fs::remove_all(apppath);
        std::cout << constant_variables.prefix << "App removed from local APM repo: " << appname << std::endl;
    }
    else
    {
        std::cout << constant_variables.prefix << "App package couldn't be found in local APM repo: " << appname << std::endl;
    }
}