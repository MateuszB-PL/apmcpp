#ifndef PKG_H
#define PKG_H

namespace pkg {

namespace constant_variables {
constexpr const char* link_base = "/";
constexpr const char* prefix = "[APM] ";
constexpr const char* app_repository_path = "/usr/apps/";
constexpr const char* system_applications_directory = "/usr/share/applications/";
}  // namespace constant_variables

class indexer {
 public:
  void indexFiles(const std::string& indexFile, const std::string& sourceDir, const std::string& replacementDir);
  void deleteIndexedFiles(const std::string& indexFile);
};

class management {
 public:
  void uninstall(std::string appname);
  void install();
};  // namespace management

}  // namespace pkg

#endif  // PKG_H
