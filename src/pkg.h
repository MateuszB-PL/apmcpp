// pkg.h

namespace pkg
{
    class application_info
    {
    public:
        json j;
        std::string appname;
        std::string pminstallcmd;
        std::string pkgarchivetype;
        std::string pmupdatecmd;
    };
    class constant_variables
    {
    public:
        std::string const link_base = "/";
        std::string const applicationdesktop = "/usr/share/applications";
        std::string const prefix = "[APM] ";
        std::string const app_repository_path = "/usr/apps";
        std::string const system_applications_directory = "/usr/share/applications/";
    };
}