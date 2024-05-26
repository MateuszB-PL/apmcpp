#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;
//vars
class vars{
    private:
        std::ifstream f;
    public:
        json j;

        std::string appname;
        std::string pminstallcmd;
        std::string pkgarchivetype;
        std::string pmupdatecmd;
        std::string prefix = "[APM] ";
        fs::path root = "/usr/apps/"+ appname +"/root";
        fs::path desktop = "/usr/apps/"+appname+"/desktop";
        fs::path link_base = "/";
        fs::path applicationdesktop = "/usr/share/applications";

        vars() {
            if (fs::exists("APPCONF")) {
                f.open("APPCONF");
                j = json::parse(f);
                appname = j["appname"];
                pminstallcmd = j["pminstallcmd"];
                pkgarchivetype = j["pkgarchivetype"];
                pmupdatecmd = j["pmupdatecmd"];
            }
        }
};