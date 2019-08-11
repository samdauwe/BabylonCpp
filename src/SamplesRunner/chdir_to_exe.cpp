#if _MSC_VER
#include <windows.h>
#include <direct.h>
#include <filesystem>
#endif

#ifdef _MSC_VER
void ChdirToExe(char *argv0)
{
  std::filesystem::path exeFolder = std::filesystem::weakly_canonical(std::filesystem::path(argv0)).parent_path();
  _chdir(exeFolder.string().c_str());
}
#else
void ChdirToExe(char *argv0)
{
}
#endif // _MSC_VER
