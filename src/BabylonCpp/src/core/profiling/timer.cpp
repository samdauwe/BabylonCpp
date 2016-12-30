#include <babylon/core/profiling/timer.h>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__linux__) || defined(__linux) || defined(linux)                 \
  || defined(__gnu_linux__)
#include <sys/resource.h>
#include <sys/time.h> /** gethrtime(), gettimeofday() */
#include <sys/times.h>
#include <time.h>   /** clock_gettime(), time() */
#include <unistd.h> /** POSIX flags */
#endif

namespace BABYLON {

/**
 * Returns the real time, in seconds, or -1.0 if an error occurred.
 *
 * Time is measured since an arbitrary and OS-dependent start time.
 * The returned real time is only useful for computing an elapsed time
 * between two calls to this function.
 */
double Timer::GetRealTime()
{
#if defined(_WIN32)
  FILETIME tm;
  ULONGLONG t;
#if defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8
  /** Windows 8, Windows Server 2012 and later. **/
  GetSystemTimePreciseAsFileTime(&tm);
#else
  /** Windows 2000 and later. **/
  GetSystemTimeAsFileTime(&tm);
#endif
  t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
  return (double)t / 10000000.0;
#elif defined(_POSIX_VERSION)
/** POSIX. **/
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  {
    struct timespec ts;
#if defined(CLOCK_MONOTONIC_RAW)
    /** Linux. **/
    const clockid_t id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_MONOTONIC)
    /** AIX, BSD, Linux, POSIX, Solaris. **/
    const clockid_t id = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
    /** AIX, BSD, HP-UX, Linux, POSIX. **/
    const clockid_t id = CLOCK_REALTIME;
#else
    const clockid_t id = (clockid_t)-1; /** Unknown. */
#endif /** CLOCK_* */
    if (id != static_cast<clockid_t>(-1) && clock_gettime(id, &ts) != -1)
      return static_cast<double>(ts.tv_sec)
             + static_cast<double>(ts.tv_nsec) / 1000000000.0;
    /** Fall thru. */
  }
#endif /** _POSIX_TIMERS */

  /** AIX, BSD, Cygwin, HP-UX, Linux, OSX, POSIX, Solaris. **/
  struct timeval tm;
  gettimeofday(&tm, NULL);
  return static_cast<double>(tm.tv_sec)
         + static_cast<double>(tm.tv_usec) / 1000000.0;
#else
  return -1.0; /** Failed. */
#endif
}

/**
 * Returns the amount of CPU time used by the current process,
 * in seconds, or -1.0 if an error occurred.
 */
double Timer::GetCPUTime()
{
#if defined(_WIN32)
  /** Windows **/
  FILETIME createTime;
  FILETIME exitTime;
  FILETIME kernelTime;
  FILETIME userTime;
  if (GetProcessTimes(GetCurrentProcess(), &createTime, &exitTime, &kernelTime,
                      &userTime)
      != -1) {
    SYSTEMTIME userSystemTime;
    if (FileTimeToSystemTime(&userTime, &userSystemTime) != -1)
      return (double)userSystemTime.wHour * 3600.0
             + (double)userSystemTime.wMinute * 60.0
             + (double)userSystemTime.wSecond
             + (double)userSystemTime.wMilliseconds / 1000.0;
  }

#elif defined(__unix__) || defined(__unix) || defined(unix)
/** AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris **/

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
  /** Prefer high-res POSIX timers, when available. */
  {
    clockid_t id;
    struct timespec ts;
#if _POSIX_CPUTIME > 0
    /** Clock ids vary by OS.  Query the id, if possible. **/
    if (clock_getcpuclockid(0, &id) == -1)
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
      /** Use known clock id for AIX, Linux, or Solaris. **/
      id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
    /** Use known clock id for BSD or HP-UX. **/
    id = CLOCK_VIRTUAL;
#else
    id                 = (clockid_t)-1;
#endif
    if (id != static_cast<clockid_t>(-1) && clock_gettime(id, &ts) != -1)
      return static_cast<double>(ts.tv_sec)
             + static_cast<double>(ts.tv_nsec) / 1000000000.0;
  }
#endif
#if defined(RUSAGE_SELF)
  {
    struct rusage rusage;
    if (getrusage(RUSAGE_SELF, &rusage) != -1)
      return static_cast<double>(rusage.ru_utime.tv_sec)
             + static_cast<double>(rusage.ru_utime.tv_usec) / 1000000.0;
  }
#endif
/*#if defined(_SC_CLK_TCK)
  {
    const double ticks = (double)sysconf(_SC_CLK_TCK);
    struct tms tms;
    if (times(&tms) != (clock_t)-1)
      return (double)tms.tms_utime / ticks;
  }
#endif
#if defined(CLOCKS_PER_SEC)
  {
    clock_t cl = clock();
    if (cl != (clock_t)-1)
      return (double)cl / (double)CLOCKS_PER_SEC;
  }
#endif*/
#endif
  return -1; /** Failed. */
}

} // end of namespace BABYLON
