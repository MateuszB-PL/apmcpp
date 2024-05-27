// pkg.h
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <filesystem>
using json = nlohmann::json;
std::ifstream f("APPCONF");

namespace pkg
{
    class jvars
    {
    public:
        json j = json::parse(f);
        std::string appname = j["appname"];
        std::string pminstallcmd = j["pminstallcmd"];
        std::string pkgarchivetype = j["pkgarchivetype"];
        std::string pmupdatecmd = j["pmupdatecmd"];
    };
    class vars
    {
    public:
        std::string const link_base = "/";
        std::string const applicationdesktop = "/usr/share/applications";
        std::string const prefix = "[APM] ";
    };
    class mgr{
        void install();
        void uninstall();
    };
}
