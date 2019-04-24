#ifndef BABYLON_CORE_FILESYSTEM_FILESYSTEM_UNIX_H
#define BABYLON_CORE_FILESYSTEM_FILESYSTEM_UNIX_H

#if !(defined(__APPLE__))
#include <linux/limits.h>
#endif
#include <string>
#include <sys/stat.h>
#include <unistd.h>

namespace BABYLON {
namespace Filesystem {

inline std::string absolutePath(const std::string& path)
{
  char tmp[PATH_MAX];
  if (realpath(path.c_str(), tmp) == NULL) {
    return "";
  }
  return std::string(tmp);
}

inline bool createDirectory(const std::string& path)
{
  return mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
}

inline std::string getcwd()
{
  char path[PATH_MAX];
  if (::getcwd(path, PATH_MAX) == NULL) {
    return "";
  }
  return std::string(path);
}

inline bool exists(const std::string& path)
{
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

inline size_t fileSize(const std::string& path)
{
  struct stat buffer;
  if (stat(path.c_str(), &buffer) != 0) {
    return 0;
  }
  return static_cast<size_t>(buffer.st_size);
}

inline bool isDirectory(const std::string& path)
{
  struct stat buffer;
  if (stat(path.c_str(), &buffer)) {
    return false;
  }
  return S_ISDIR(buffer.st_mode);
}

inline bool isFile(const std::string& path)
{
  struct stat buffer;
  if (stat(path.c_str(), &buffer)) {
    return false;
  }
  return S_ISREG(buffer.st_mode);
}

inline bool removeFile(const std::string& path)
{
  return std::remove(path.c_str()) == 0;
}

} // end of namespace Filesystem
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_FILESYSTEM_FILESYSTEM_UNIX_H
