#pragma once
#if defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#elif defined(_WIN32)
#include <direct.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include "anim.hpp"

namespace io {
bool exists(std::string filename);
void makedir(std::string filename);
unsigned int get_filesize(std::string filename);

}  // namespace io
