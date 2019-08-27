#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else // _WIN32
#include <libgen.h>
#endif // _WIN32
#include <babylon/core/filesystem.h>
#include <babylon/core/system.h>

namespace BABYLON {
namespace System {

std::string getExecutablePath()
{
#ifdef _WIN32
  char filename[3000];
  GetModuleFileName(NULL, filename, 3000);
  return std::string(filename);
#elif __APPLE__
  return "./";
#else
  char filename[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", filename, PATH_MAX);
  (void) count;
  return std::string(filename);
#endif
}

void chdirToExecutableFolder()
{
  std::string exeFolder = BABYLON::Filesystem::baseDir(getExecutablePath());
#ifdef _WIN32
  _chdir(exeFolder.c_str());
#else
  chdir(exeFolder.c_str());
#endif
}

void openBrowser(const std::string &url)
{
#ifndef _WIN32
  std::string cmd = std::string("open ") + url;
  system(cmd.c_str());
#else
  ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
#endif
}

void openFile(const std::string &filename)
{
#ifndef _WIN32
  std::string cmd = std::string("open ") + BABYLON::Filesystem::absolutePath(filename);
  system(cmd.c_str());
#else
  std::string canonical_path = BABYLON::Filesystem::absolutePath(filename);
  ShellExecute(0, 0, canonical_path.c_str(), 0, 0, SW_SHOW);
#endif
}

} // end of namespace System
} // end of namespace BABYLON
