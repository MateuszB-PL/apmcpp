#include "pkg.h"
// pkg.cpp

// pkg
void create_recursive_symlink(const fs::path &target, const fs::path &link) {
  try {
    if (!fs::exists(link)) {
      fs::create_symlink(target, link);
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Error creating symlink: " << e.what() << std::endl;
  }
}

void forExecuteCommands(const std::string &jsonarray) {
  for (std::string value : application_info.j[jsonarray]) {
    system(value.c_str());
  }
}

std::string replacePathPrefix(const std::string &originalPath, const std::string &oldPrefix, const std::string &newPrefix) {
  if (originalPath.find(oldPrefix) == 0) {
    std::string newPath = originalPath;
    newPath.replace(0, oldPrefix.length(), newPrefix);
    return newPath;
  } else {
    return originalPath;
  }
}

void indexFiles(const std::string &indexFile, const std::string &sourceDir, const std::string &replacementDir) {
  std::ofstream outFile(indexFile, std::ios::app);
  if (!outFile.is_open()) {
    std::cerr << "Cannot open INDEXFILE" << indexFile << std::endl;
    return;
  }

  try {
    if (fs::exists(sourceDir) && fs::is_directory(sourceDir)) {
      for (const auto &entry : fs::recursive_directory_iterator(sourceDir)) {
        if (fs::is_regular_file(entry)) {
          std::string filePath = entry.path().string();
          std::string newFilePath = replacePathPrefix(filePath, sourceDir, replacementDir);
          outFile << newFilePath << std::endl;
        }
      }
    }

  } catch (const fs::filesystem_error &e) {
    std::cerr << "Filesystem error while indexing" << e.what() << std::endl;
    exit(1);
  }

  outFile.close();
}

void deleteIndexedFiles(const std::string &indexFile) {
  std::ifstream inFile(indexFile);
  if (!inFile.is_open()) {
    std::cerr << "Cannot read FILEINDEX " << indexFile << std::endl;
    return;
  }

  std::string filePath;
  while (std::getline(inFile, filePath)) {
    try {
      if (fs::exists(filePath)) fs::remove(filePath);
    } catch (const fs::filesystem_error &e) {
      std::cerr << "Filesystem error while removing: " << e.what() << std::endl;
      exit(1);
    }
  }

  inFile.close();
  std::cout << "Removed all indexed files." << std::endl;
}
void linkFiles() {
  try {
    std::string root = pkg::constant_variables::app_repository_path + application_info.appname + "/root/";
    std::string desktop = pkg::constant_variables::app_repository_path + application_info.appname + "/desktop/";
    if (fs::exists(root) && fs::is_directory(root)) {
      for (const auto &entry : fs::recursive_directory_iterator(root)) {
        fs::path target = entry.path();
        fs::path link = pkg::constant_variables::link_base / fs::relative(target, root);
        create_recursive_symlink(target, link);
      }
    }
    if (fs::exists(desktop) && fs::is_directory(desktop)) {
      for (const auto &entry : fs::recursive_directory_iterator(desktop)) {
        fs::path target = entry.path();
        fs::path link = pkg::constant_variables::system_applications_directory / fs::relative(target, desktop);
        create_recursive_symlink(target, link);
      }
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << pkg::constant_variables::prefix << "Error while linking files: " << e.what() << std::endl;
    exit(1);
  }
}
void loadAPPCONF() {
  std::ifstream f("APPCONF");
  try {
    application_info.j = json::parse(f);
  } catch (nlohmann::json::parse_error &e) {
    std::cerr << "This file contains syntax errors!" << std::endl;
    exit(1);
  }
  try {
    application_info.appname = application_info.j["appname"];
    application_info.pkgarchivetype = application_info.j["pkgarchivetype"];
    application_info.appversion = application_info.j["appversion"];
  } catch (nlohmann::json::type_error &e) {
    std::cerr << "This APPCONF is not configured properly!" << std::endl;
    exit(1);
  }
  int APPCONFVERSION = application_info.j["APPCONFVERSION"];
  if (APPCONFVERSION != 2) {
    std::cout << "This APM only supports version 2 of APPCONF. Current version: " << std::to_string(APPCONFVERSION) << ". Please update APPCONF or APM!" << std::endl;
    exit(3);
  }
}

void exitUserInterrupt() {
  std::cout << "Bye!" << std::endl;
  exit(0);
}

void install() {
  loadAPPCONF();

  std::cout << std::endl << std::endl;
  std::cout << "Continue? [Y/n]: ";

  char userinput = std::cin.get();
  if (tolower(userinput) == 'n') {
    exitUserInterrupt();
  }

  std::cout << pkg::constant_variables::prefix << "Executing preparation commands... " << std::endl;

  forExecuteCommands("prep");

  std::cout << pkg::constant_variables::prefix << "Please wait, extraction in progress, this might take a while..." << std::endl;
  std::string extractstr = application_info.appname + "." + std::string(application_info.pkgarchivetype);

  extract(extractstr.c_str());

  const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
  std::filesystem::copy(application_info.appname, pkg::constant_variables::app_repository_path + application_info.appname, copyOptions);
  std::filesystem::remove_all(application_info.appname);

  std::cout << pkg::constant_variables::prefix << "Extracting and Copying done" << std::endl;

  std::string FILEINDEX = pkg::constant_variables::app_repository_path + application_info.appname + "/FILEINDEX";
  std::string root_directory = pkg::constant_variables::app_repository_path + application_info.appname + "/root/";
  std::string desktop_file_directory = pkg::constant_variables::app_repository_path + application_info.appname + "/desktop/";

  indexFiles(FILEINDEX, root_directory, "/");
  indexFiles(FILEINDEX, desktop_file_directory, pkg::constant_variables::system_applications_directory);
  std::cout << pkg::constant_variables::prefix << "Successfully indexed files" << std::endl;

  linkFiles();
  std::cout << pkg::constant_variables::prefix << "Successfully linked files" << std::endl;

  std::cout << pkg::constant_variables::prefix << "Executing post-install commands..." << std::endl;
  forExecuteCommands("post");

  std::cout << pkg::constant_variables::prefix << "Installation Complete!" << std::endl;
}

void uninstall(std::string appname) {
  std::string apppath = pkg::constant_variables::app_repository_path + appname;
  if (fs::exists(apppath) && fs::is_directory(apppath)) {
    deleteIndexedFiles(apppath + "/FILEINDEX");
    fs::remove_all(apppath);
    std::cout << pkg::constant_variables::prefix << "App removed from local APM repo: " << appname << std::endl;
  } else {
    std::cout << pkg::constant_variables::prefix << "App package couldn't be found in local APM repo: " << appname << std::endl;
  }
}
