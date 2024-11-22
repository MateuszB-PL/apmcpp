void generate_example_appconf() {
  std::ofstream outfile("rename_to_APPCONF");

  outfile << R"({
    "appname":"example",
    "appversion": "1.0.0",
    "pkgarchivetype": "tar.gz",
    "prep": [
        "echo <Commands that will be executed before installation>"
    ],
    "post": [
        "echo <Commands that will be executed after installation>"
    ],
    "APPCONFVERSION": 2
}
)" << std::endl;

  outfile.close();
}

void display_installed_packages() {
  for (const auto &entry : fs::directory_iterator(pkg::constant_variables::app_repository_path)) std::cout << entry.path() << std::endl;
}