// pkg.h

namespace pkg
{
    class jvars
    {
    public:
        json j;
        std::string appname;
        std::string pminstallcmd;
        std::string pkgarchivetype;
        std::string pmupdatecmd;
        std::string appsrc;
    };
    class vars
    {
    public:
        std::string const link_base = "/";
        std::string const applicationdesktop = "/usr/share/applications";
        std::string const prefix = "[APM] ";
    };
}
