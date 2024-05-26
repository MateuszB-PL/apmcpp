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
        std::string appname;
        std::string pminstallcmd;
        std::string pkgarchivetype;
        std::string pmupdatecmd;
    };
    class vars
    {
    public:
        std::string link_base = "/";
        std::string root;
        std::string desktop;
        std::string applicationdesktop;
        std::string apppath;
        std::string const prefix = "[APM] ";
    };
    class mgr{
        void install();
        void uninstall();
    };
}
