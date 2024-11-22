#ifndef PKG_H
#define PKG_H

namespace pkg {
class application_info {
 public:
  nlohmann::json j;
  std::string appname;
  std::string appversion;
  std::string pkgarchivetype;
};

namespace constant_variables {
constexpr const char* link_base = "/";
constexpr const char* prefix = "[APM] ";
constexpr const char* app_repository_path = "/usr/apps/";
constexpr const char* system_applications_directory = "/usr/share/applications/";
}  // namespace constant_variables
}  // namespace pkg

#endif  // PKG_H

void linkFiles();
