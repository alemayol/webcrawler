#include "../model/Crawler.h"
#include <string>
#include <unordered_map>
class GestorJSON {
public:
  static bool guardarArchivo(std::string nombreA, Crawler *crawler);
  static bool cargarArchivo(std::string nombreA, Crawler *crawler);

private:
};
