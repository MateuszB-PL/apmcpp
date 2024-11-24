

void acm::management::APPCONFinit() {
  std::ifstream f("APPCONF");

  try {
    mgr.j = json::parse(f);
  } catch (nlohmann::json::parse_error &e) {
    std::cerr << "This file contains syntax errors!" << std::endl;
    exit(1);
  }
  int APPCONFVERSION = mgr.j["APPCONFVERSION"];
  if (APPCONFVERSION != 3) {
    std::cout << "This APM only supports version 3 of APPCONF. Current version: " << std::to_string(APPCONFVERSION) << ". Please update APPCONF or APM!" << std::endl;
    exit(3);
  }
}

auto acm::management::APPCONFvalue(std::string x) {
  try {
    return mgr.j[x];
  } catch (nlohmann::json::type_error &e) {
    std::cerr << "This APPCONF is not configured properly!" << std::endl;
    exit(1);
  }
}
