#include "vars.h"
namespace fs = std::filesystem;

using json = nlohmann::json;

vars::vars::vars() {
    std::ifstream f;
    if (fs::exists("APPCONF")) {
        f.open("APPCONF");
        j = json::parse(f);
        appname = j["appname"];
        pminstallcmd = j["pminstallcmd"];
        pkgarchivetype = j["pkgarchivetype"];
        pmupdatecmd = j["pmupdatecmd"];
        

    }
    f.close();
}
