#ifndef BABYLON_CORE_FUTURE_THEN_H
#define BABYLON_CORE_FUTURE_THEN_H

#include <future>
#include <type_traits>
#include <utility>

namespace BABYLON {

/**
 * @brief Experimental implementation of future::then()
 * @see https://github.com/jaredhoberock/then
 */
template <class T, class Function>
std::future<typename std::result_of<Function(std::future<T>&)>::type>
then(std::future<T>& fut, std::launch policy, Function&& f)
{
  return std::async(policy,
                    [](std::future<T>&& fut, Function&& f) {
                      fut.wait();
                      return std::forward<Function>(f)(fut);
                    },
                    std::move(fut), std::forward<Function>(f));
}

template <class T, class Function>
std::future<typename std::result_of<Function(std::future<T>&)>::type>
then(std::future<T>& fut, Function&& f)
{
  return then(fut, std::launch::async | std::launch::deferred,
              std::forward<Function>(f));
}

} // end of namespace BABYLON

#endif // BABYLON_CORE_FUTURE_THEN_H
