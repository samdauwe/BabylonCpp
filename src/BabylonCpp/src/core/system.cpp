#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else // _WIN32
#include <libgen.h>
#endif // _WIN32
#include <babylon/core/filesystem.h>
#include <babylon/core/logging.h>
#include <babylon/core/system.h>
#include <babylon/misc/string_tools.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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
  (void)count;
  return std::string(filename);
#endif
}

void chdirToExecutableFolder()
{
  std::string exeFolder = BABYLON::Filesystem::baseDir(getExecutablePath());
#ifdef _WIN32
  _chdir(exeFolder.c_str());
#else
  int result = chdir(exeFolder.c_str());
  if (result != 0)
    BABYLON_LOG_WARN("system.cpp", "chdir to %s failed", exeFolder.c_str())
#endif
}

void openBrowser(const std::string& url)
{
#if defined(_WIN32)
  ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
#elif defined(__EMSCRIPTEN__)
  std::string jsCode = "window.open('url','_blank');";
  jsCode             = StringTools::replace(jsCode, "url", url);
  emscripten_run_script(jsCode.c_str());
#else
  std::string cmd = std::string("open ") + url;
  int result      = system(cmd.c_str());
  if (result != 0)
    BABYLON_LOG_WARN("system.cpp", "system(%S) failed", cmd.c_str())
#endif
}

void openFile(const std::string& filename)
{
#ifndef _WIN32
  std::string cmd = std::string("open ") + BABYLON::Filesystem::absolutePath(filename);
  int result      = system(cmd.c_str());
  if (result != 0)
    BABYLON_LOG_WARN("system.cpp", "system(%S) failed", cmd.c_str())
#else
  std::string canonical_path = BABYLON::Filesystem::absolutePath(filename);
  ShellExecute(0, 0, canonical_path.c_str(), 0, 0, SW_SHOW);
#endif
}

} // end of namespace System
} // end of namespace BABYLON
