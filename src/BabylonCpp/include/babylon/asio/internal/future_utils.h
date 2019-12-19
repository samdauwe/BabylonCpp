//
// Created by Pascal Thomet on 19/12/2019.
//

#ifndef BABYLONCPP_FUTURE_UTILS_H
#define BABYLONCPP_FUTURE_UTILS_H

#include <future>

namespace BABYLON {
namespace asio {

template <typename F, typename... Ts>
inline auto really_async(F&& f, Ts&&... params)
{
  return std::async(std::launch::async, std::forward<F>(f), std::forward<Ts>(params)...);
}

enum class future_running_status { future_not_launched, future_running, future_ready };
template <typename T>
future_running_status get_future_running_status(std::future<T>& v)
{
  if (!v.valid())
    return future_running_status::future_not_launched;
  if (v.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    return future_running_status::future_ready;
  else
    return future_running_status::future_running;
}

} // namespace asio
} // namespace BABYLON
#endif // BABYLONCPP_FUTURE_UTILS_H
