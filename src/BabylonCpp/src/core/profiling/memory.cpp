#include <babylon/core/profiling/memory.h>

#if defined(_WIN32)
#include <Windows.h>
#include <psapi.h>
#elif defined(__linux__) || defined(__linux) || defined(linux)                 \
  || defined(__gnu_linux__)
#include <stdio.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace BABYLON {

/**
 * Returns the size of physical memory (RAM) in bytes.
 */
size_t Memory::GetMemorySize()
{
#if defined(_WIN32) && (defined(__CYGWIN__) || defined(__CYGWIN32__))
  /** Cygwin under Windows. **/
  // New 64-bit MEMORYSTATUSEX isn't available.  Use old 32.bit
  MEMORYSTATUS status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatus(&status);
  return (size_t)status.dwTotalPhys;
#elif defined(_WIN32)
  /** Windows **/
  // Use new 64-bit MEMORYSTATUSEX, not old 32-bit MEMORYSTATUS
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  GlobalMemoryStatusEx(&status);
  return (size_t)status.ullTotalPhys;
#elif defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)
  /** FreeBSD, Linux, OpenBSD, and Solaris. **/
  return static_cast<size_t>(sysconf(_SC_PHYS_PAGES))
         * static_cast<size_t>(sysconf(_SC_PAGESIZE));
#else
  /** Unknown OS **/
  return 0L;
#endif
}

/**
 * Returns the peak (maximum so far) resident set size (physical
 * memory use) measured in bytes, or zero if the value cannot be
 * determined on this OS.
 */
size_t Memory::GetPeakRSS()
{
#if defined(_WIN32)
  /** Windows **/
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
  return (size_t)info.PeakWorkingSetSize;
#elif defined(__linux__) || defined(__linux) || defined(linux)                 \
  || defined(__gnu_linux__)
  /** BSD, Linux, and OSX **/
  struct rusage rusage;
  getrusage(RUSAGE_SELF, &rusage);
  return static_cast<size_t>(rusage.ru_maxrss * 1024L);
#else
  /** Unknown OS **/
  return (size_t)0L; /* Unsupported. */
#endif
}

/**
 * Returns the current resident set size (physical memory use) measured
 * in bytes, or zero if the value cannot be determined on this OS.
 * http://nadeausoftware.com/articles/c_c
 */
size_t Memory::GetCurrentRSS()
{
#if defined(_WIN32)
  /** Windows **/
  PROCESS_MEMORY_COUNTERS info;
  GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
  return (size_t)info.WorkingSetSize;
#elif defined(__linux__) || defined(__linux) || defined(linux)                 \
  || defined(__gnu_linux__)
  /** Linux **/
  long rss = 0L;
  FILE* fp = NULL;
  if ((fp = fopen("/proc/self/statm", "r")) == NULL)
    return static_cast<size_t>(0L); /* Can't open? */
  if (fscanf(fp, "%*s%ld", &rss) != 1) {
    fclose(fp);
    return static_cast<size_t>(0L); /* Can't read? */
  }
  fclose(fp);
  return static_cast<size_t>(rss) * static_cast<size_t>(sysconf(_SC_PAGESIZE));
#else
  /** AIX, BSD, Solaris, and Unknown OS **/
  return (size_t)0L; /* Unsupported. */
#endif
}

} // end of namespace BABYLON
