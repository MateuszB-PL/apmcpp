// pkg.cpp

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


std::string replacePathPrefix(const std::string& originalPath, const std::string& oldPrefix, const std::string& newPrefix) {
    if (originalPath.find(oldPrefix) == 0) {
        std::string newPath = originalPath;
        newPath.replace(0, oldPrefix.length(), newPrefix);
        return newPath;
    } else {
        return originalPath;
    }
}

// Funkcja do indeksowania plików z dopisaniem do istniejącego pliku
void indexFiles(const std::string& indexFile, const std::string& sourceDir, const std::string& replacementDir) {
    std::ofstream outFile(indexFile, std::ios::app); // Otwieramy plik do zapisu z dopisaniem (append)
    if (!outFile.is_open()) {
        std::cerr << "Cannot open INDEXFILE" << indexFile << std::endl;
        return;
    }

    try {
        for (const auto& entry : fs::recursive_directory_iterator(sourceDir)) {
            if (fs::is_regular_file(entry)) {
                std::string filePath = entry.path().string();
                std::string newFilePath = replacePathPrefix(filePath, sourceDir, replacementDir);
                outFile << newFilePath << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error while indexing" << e.what() << std::endl;
    }

    outFile.close();
}
void deleteIndexedFiles(const std::string& indexFile) {
    std::ifstream inFile(indexFile);
    if (!inFile.is_open()) {
        std::cerr << "Cannot read FILEINDEX " << indexFile << std::endl;
        return;
    }

    std::string filePath;
    while (std::getline(inFile, filePath)) {
        try {
            fs::remove(filePath);
            std::cout << "Removed: " << filePath << std::endl;
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error while removing: " << e.what() << std::endl;
        }
    }

    inFile.close();
    std::cout << "Removed all indexed files." << std::endl;
}
void install() {
    jvars.j = json::parse(f);
    jvars.appname = jvars.j["appname"];
    jvars.pminstallcmd = jvars.j["pminstallcmd"];
    jvars.pkgarchivetype = jvars.j["pkgarchivetype"];
    jvars.pmupdatecmd = jvars.j["pmupdatecmd"];
    jvars.appsrc = jvars.j["appsrc"];

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
        
        std::string indexFile = "/usr/apps/"+jvars.appname+"/FILEINDEX";
        std::string rootdir = "/usr/apps/"+jvars.appname+"/root/";
        std::string desktopdir = "/usr/apps/"+jvars.appname+"/desktop/";
        std::string applications = "/usr/share/applications/";

        // Indeksowanie plików
        indexFiles(indexFile, rootdir, "/"); // Indeksowanie bez zmiany prefiksu
        indexFiles(indexFile, desktopdir, applications); 
        try
        {
            std::string root = "/usr/apps/" + jvars.appname + "/root";
            std::string desktop = "/usr/apps/" + jvars.appname + "/desktop";
            if (fs::exists(root) || fs::is_directory(root))
            {
                for (const auto &entry : fs::recursive_directory_iterator(root))
                {
                    fs::path target = entry.path();
                    fs::path link = vars.link_base / fs::relative(target, root);
                    create_recursive_symlink(target, link);
                }
                std::cout << vars.prefix << "System root files linked to /.\n"
                          << std::endl;
            }
            if (fs::exists(desktop) || fs::is_directory(desktop))
            {
                for (const auto &entry : fs::recursive_directory_iterator(desktop))
                {
                    fs::path target = entry.path();
                    fs::path link = vars.applicationdesktop / fs::relative(target, desktop);
                    create_recursive_symlink(target, link);
                }
                std::cout << vars.prefix << "Desktop files linked to:" << vars.applicationdesktop
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
    std::string apppath = "/usr/apps/" + appname;
    if (fs::exists(apppath) || fs::is_directory(apppath))
    {
        deleteIndexedFiles(apppath+"/FILEINDEX");   
        fs::remove_all(apppath);
        std::cout << vars.prefix << "App removed from local APM repo: " << appname << std::endl;
    }
    else
    {
        std::cout << vars.prefix << "App package couldn't be found in local APM repo: " << appname << std::endl;
    }
}