#ifndef BABYLON_CORE_TIME_H
#define BABYLON_CORE_TIME_H

#include <iomanip>
#include <sstream>

#include <babylon/babylon_common.h>

namespace BABYLON {
namespace Time {

inline system_time_point_t systemTimepointNow()
{
  return std::chrono::system_clock::now();
}

inline high_res_time_point_t highresTimepointNow()
{
  return std::chrono::high_resolution_clock::now();
}

inline tm localtime(const std::time_t& time)
{
  struct tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32)                                         \
     || defined(__WIN32__) && !defined(__GNUC__))
  localtime_s(&tm_snapshot, &time); // windsows
#else
  localtime_r(&time, &tm_snapshot); // POSIX
#endif
  return tm_snapshot;
}

inline system_time_point_t makeTimePoint(int year, int mon, int day, int hour,
                                         int min, int sec)
{
  struct std::tm t;
  t.tm_sec       = sec;  // second of minute (0 .. 59 and 60 for leap seconds)
  t.tm_min       = min;  // minute of hour (0 .. 59)
  t.tm_hour      = hour; // hour of day (0 .. 23)
  t.tm_mday      = day;  // day of month (0 .. 31)
  t.tm_mon       = mon - 1;     // month of year (0 .. 11)
  t.tm_year      = year - 1900; // year since 1900
  t.tm_isdst     = -1;          // determine whether daylight saving time
  std::time_t tt = std::mktime(&t);
  if (tt == -1) {
    // No valid system time
    return system_time_point_t();
  }
  return std::chrono::system_clock::from_time_t(tt);
}

/**
 * Meaures the duration of the function call.
 */
template <typename Duration, typename Functor, typename... Args>
auto measureFunctionCall(Duration& duration, Functor f, Args&&... args)
  -> decltype(f(std::forward<Args>(args)...))
{
  struct scoped_timer {
    scoped_timer(Duration& duration)
        : _now{std::chrono::high_resolution_clock::now()}, _d{duration}
    {
    }
    ~scoped_timer()
    {
      _d = std::chrono::high_resolution_clock::now() - _now;
    }

  private:
    std::chrono::high_resolution_clock::time_point const _now;
    Duration& _d;
  } scoped_timer(duration);

  return f(std::forward<Args>(args)...);
}

/**
 * @brief Returns the fractional duration since high_res_timepoint.
 * @param high_res_timepoint
 * @return fractional duration since high_res_timepoint
 */
template <typename T, typename Period>
inline T fpTimeSince(const high_res_time_point_t& high_res_timepoint)
{
  std::chrono::duration<double, Period> fp_ms
    = std::chrono::high_resolution_clock::now() - high_res_timepoint;
  return static_cast<T>(fp_ms.count());
}

template <typename T, typename Period>
inline T fpTimeDiff(const high_res_time_point_t& t0,
                    const high_res_time_point_t& t1)
{
  std::chrono::duration<double, Period> fp_ms = t1 - t0;
  return static_cast<T>(fp_ms.count());
}

template <typename T>
inline T fpMillisecondsDuration(const microsecond_t& d)
{
  return static_cast<T>(d.count()) / static_cast<T>(1000.f);
}

/**
 * @brief Produces an ISO-8601 string representation of the timestamp
 * @param timestamp the epoch time in seconds
 * @param include_timezone appends Z UTC flag at end of string if true
 * @return a string representing the timestamp in UTC
 */
inline std::string toIso8601(std::time_t timestamp,
                             const bool& include_timezone = false)
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(                                                               \
  disable : 4996) // 'gmtime': This function or variable may be unsafe.
#endif
  std::tm exploded_time(*std::gmtime(&timestamp));
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  std::string ret;

  if (include_timezone) {
    char buf[sizeof("1970-01-01T00:00:00Z")];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &exploded_time);
    ret = buf;
  }
  else {
    char buf[sizeof("1970-01-01T00:00:00")];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &exploded_time);
    ret = buf;
  }

  return ret;
}

inline std::string toIso8601Ms(const system_time_point_t& system_time_point)
{
  std::time_t timestamp
    = std::chrono::system_clock::to_time_t(system_time_point);
  auto ms = std::chrono::duration_cast<milliseconds_t>(
              system_time_point.time_since_epoch())
              .count();
  std::size_t _ms = static_cast<std::size_t>(ms % 1000);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(                                                               \
  disable : 4996) // 'localtime': This function or variable may be unsafe.
#endif
  // Format timestamp to 'YYYY-MM-DD HH:MM:SS'
  std::tm* ptm = std::localtime(&timestamp);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  char buffer[32];
  std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
  // Append microseonds
  std::ostringstream oss;
  oss << buffer << "." << std::setfill('0') << std::setw(3) << _ms;
  return oss.str();
}

/**
 * @brief Returns the number of milliseconds since the Unix epoch. Equivalent
 * function of JavaScript function getTime().
 * @return the number of milliseconds since the Unix epoch
 */
inline long unixtimeInMs()
{
  using namespace std::chrono;
  return static_cast<long>(
    duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch())
      .count());
}

/**
 * @brief Calculate the current UTC time stamp
 * @return the UTC time stamp
 */
inline std::time_t utcTime()
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(                                                               \
  disable : 4996) // 'localtime': This function or variable may be unsafe.
#endif
  std::time_t now = std::time(NULL);

  std::tm tm_local(*std::localtime(&now));
  std::time_t t_local = std::mktime(&tm_local);

  std::tm tm_utc(*std::gmtime(&t_local));
  std::time_t t_utc = std::mktime(&tm_utc);
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  return now - (t_utc - t_local);
}

} // end of namespace Time
} // end of namespace BABYLON

#endif // end of BABYLON_CORE_TIME_H
