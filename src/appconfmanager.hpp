#ifndef ACM_H
#define ACM_H

namespace acm {
class management {
 public:
  nlohmann::json j;
  void APPCONFinit();
  auto APPCONFvalue(std::string x);
};
class data {
 public:
  std::string appname;
  std::string appversion;
  std::string pkgarchivetype;
  std::string description;
};
}  // namespace acm
#endif  // ACM_H
