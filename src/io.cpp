#include "io.hpp"

namespace io {
bool exists(std::string filename) {
  struct stat st = {0};
  if (stat(filename.c_str(), &st) != -1) {
    return (true);
  }
  return (false);
}
void makedir(std::string filename) {
#if defined(__APPLE__) || defined(__linux__)
  mkdir(filename.c_str(), 0700);
#elif defined(_WIN32)
  _mkdir(filename.c_str());
#endif
}
unsigned int get_filesize(std::string filename) {
  struct stat st = {0};
  if (stat(filename.c_str(), &st) != -1) {
    return (st.st_size);
  }
  return (0);
}

}  // namespace io
