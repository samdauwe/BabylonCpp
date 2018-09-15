#ifndef BABYLON_CORE_FILESYSTEM_FILESYSTEM_WIN32_H
#define BABYLON_CORE_FILESYSTEM_FILESYSTEM_WIN32_H

#include <Shlwapi.h>
#include <iostream>
#include <string>
#include <windows.h>

#ifdef _MSC_VER
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#endif

namespace BABYLON {
namespace Filesystem {

inline std::string absolutePath(const std::string& path)
{
  char tmp[PATH_MAX];
  GetFullPathName(path.c_str(), MAX_PATH, tmp, nullptr);
  return std::string(tmp);
}

inline bool createDirectory(const std::string& path)
{
  return (CreateDirectory(path.c_str(), NULL)
          || ERROR_ALREADY_EXISTS == GetLastError());
}

inline std::string getcwd()
{
  char path[MAX_PATH] = "";
  GetCurrentDirectoryA(MAX_PATH, path);
  PathAddBackslashA(path);
  return path;
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
  return (buffer.st_mode & S_IFDIR);
}

inline bool isFile(const std::string& path)
{
  struct stat buffer;
  if (stat(path.c_str(), &buffer)) {
    return false;
  }
  return (buffer.st_mode & S_IFREG);
}

inline bool removeFile(const std::string& path)
{
  return std::remove(path.c_str()) == 0;
}

} // end of namespace Filesystem
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_FILESYSTEM_FILESYSTEM_WIN32_H
