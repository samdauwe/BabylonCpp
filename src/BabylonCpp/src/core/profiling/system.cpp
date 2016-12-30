#include <babylon/core/profiling/system.h>

#if defined(_WIN32)
#include <Windows.h>
#include <psapi.h>
#elif defined(__linux__) || defined(__linux) || defined(linux)                 \
  || defined(__gnu_linux__)
#include <stdio.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#endif

namespace BABYLON {

int System::GetCurrentPID()
{
#if defined(_WIN32) && (defined(__CYGWIN__) || defined(__CYGWIN32__))
  /** Cygwin under Windows. **/
  pid_t pid;
  pid = getpid();
  return (int)pid;
#elif defined(_WIN32)
  /** Windows. **/
  DWORD pid;
  pid = GetCurrentProcessId();
  return (int)pid;
#elif defined(__linux__) || defined(__linux) || defined(linux)                 \
  || defined(__gnu_linux__)
  pid_t pid;
  pid = getpid();
  return static_cast<int>(pid);
#else
  return 0;
#endif
}

const char* System::GetOperatingSystem()
{
#if defined(__CYGWIN__) && !defined(_WIN32)
  /** Cygwin POSIX under Microsoft Windows. **/
  return "Cygwin";
#elif defined(_WIN64)
  /** Microsoft Windows (64-bit). **/
  return "WIN64";
#elif defined(_WIN32)
  /** Microsoft Windows (32-bit). **/
  return "WIN32";
#elif defined(__linux__)
  /** Linux. **/
  return "Linux";
#else
  /** Unknown OS **/
  return "Unknown";
#endif
}

const char* System::GetProcessorArch()
{
#if defined(__x86_64__) || defined(_M_X64)
  /** x86 64-bit **/
  return "x86 64-bit";
#elif defined(__i386) || defined(_M_IX86)
  /** x86 32-bit **/
  return "x86 32-bit";
#else
  /** Unknown OS **/
  return "Unknown";
#endif
}

} // end of namespace BABYLON
