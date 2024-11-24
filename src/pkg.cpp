// pkg.cpp

// pkg
#include "pkg.hpp"

#include <bits/this_thread_sleep.h>

#include <tabulate/table.hpp>

void forExecuteCommands(const std::string &jsonarray) {
  for (std::string value : mgr.APPCONFvalue(jsonarray)) {
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

void pkg::indexer::indexFiles(const std::string &indexFile, const std::string &sourceDir, const std::string &replacementDir) {
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

void pkg::indexer::deleteIndexedFiles(const std::string &indexFile) {
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
// Link all files
void create_recursive_symlink(const fs::path &target, const fs::path &link) {
  try {
    if (!fs::exists(link)) {
      fs::create_symlink(target, link);
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Error creating symlink: " << e.what() << std::endl;
  }
}
void linkFiles() {
  try {
    std::string root = pkg::constant_variables::app_repository_path + appconf.appname + "/root/";
    std::string desktop = pkg::constant_variables::app_repository_path + appconf.appname + "/desktop/";
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

//
// exit
void exitUserInterrupt() {
  std::cout << "Bye!" << std::endl;
  exit(0);
}
//
// progress bar
void showProgressBar(float progress, int barWidth = 30) {
  int pos = static_cast<int>(barWidth * progress);
  std::cout << "[";
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos) {
      std::cout << "=";
    } else if (i == pos) {
      std::cout << ">";
    } else {
      std::cout << " ";
    }
  }
  std::cout << "] " << int(progress * 100.0) << " %\r";
  std::cout.flush();
}
void progress(float percent) {
  for (int i = 0; i <= 100; ++i) {
    float currentProgress = (i / 100.0f) * percent;
    showProgressBar(currentProgress);
  }
  std::cout << std::endl;
  std::cout << std::endl;
}
//
void pkg::management::install() {
  mgr.APPCONFinit();
  appconf.appname = mgr.APPCONFvalue("appname");
  appconf.appversion = mgr.APPCONFvalue("appversion");
  appconf.pkgarchivetype = mgr.APPCONFvalue("pkgarchivetype");
  appconf.description = mgr.APPCONFvalue("description");
  tabulate::Table package_info;
  package_info.add_row({"Name", "Version", "Description"});
  package_info.add_row({appconf.appname, appconf.appversion, appconf.description});
  std::cout << package_info << std::endl;

  std::cout << "Continue? [Y/n]: ";

  char userinput = std::cin.get();
  if (tolower(userinput) == 'n') {
    exitUserInterrupt();
  }
  progress(0);

  std::cout << pkg::constant_variables::prefix << "Executing preparation commands... " << std::endl;

  forExecuteCommands("prep");
  progress(0.14);
  std::cout << pkg::constant_variables::prefix << "Please wait, extraction in progress, this might take a while..." << std::endl;
  std::string extractstr = appconf.appname + "." + std::string(appconf.pkgarchivetype);

  extract(extractstr.c_str());
  progress(0.28);

  const auto copyOptions = std::filesystem::copy_options::update_existing | std::filesystem::copy_options::recursive;
  std::filesystem::copy(appconf.appname, pkg::constant_variables::app_repository_path + appconf.appname, copyOptions);
  std::filesystem::remove_all(appconf.appname);

  std::cout << pkg::constant_variables::prefix << "Extracting and Copying done" << std::endl;
  progress(0.42);

  std::string FILEINDEX = pkg::constant_variables::app_repository_path + appconf.appname + "/FILEINDEX";
  std::string root_directory = pkg::constant_variables::app_repository_path + appconf.appname + "/root/";
  std::string desktop_file_directory = pkg::constant_variables::app_repository_path + appconf.appname + "/desktop/";

  pkgIndexer.indexFiles(FILEINDEX, root_directory, "/");
  pkgIndexer.indexFiles(FILEINDEX, desktop_file_directory, pkg::constant_variables::system_applications_directory);
  std::cout << pkg::constant_variables::prefix << "Successfully indexed files" << std::endl;
  progress(0.71);

  linkFiles();
  std::cout << pkg::constant_variables::prefix << "Successfully linked files" << std::endl;
  progress(0.85);

  std::cout << pkg::constant_variables::prefix << "Executing post-install commands..." << std::endl;
  forExecuteCommands("post");
  progress(1);

  std::cout << pkg::constant_variables::prefix << "Installation Complete!" << std::endl;
}

void pkg::management::uninstall(std::string appname) {
  std::string apppath = pkg::constant_variables::app_repository_path + appname;
  if (fs::exists(apppath) && fs::is_directory(apppath)) {
    progress(0);
    pkgIndexer.deleteIndexedFiles(apppath + "/FILEINDEX");

    progress(0.5);
    fs::remove_all(apppath);

    progress(1);

    std::cout << pkg::constant_variables::prefix << "App removed from local APM repo: " << appname << std::endl;
  } else {
    std::cout << pkg::constant_variables::prefix << "App package couldn't be found in local APM repo: " << appname << std::endl;
  }
}
