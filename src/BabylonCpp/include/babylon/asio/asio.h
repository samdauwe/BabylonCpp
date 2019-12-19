#ifndef BABYLON_ASYNC_REQUESTS_H
#define BABYLON_ASYNC_REQUESTS_H

#include <babylon/babylon_common.h>
#include <babylon/asio/callback_types.h>
#include <variant>
#include <functional>
#include <string>

namespace BABYLON {
namespace asio {


/**
 * @brief LoadUrlAsync_Text will load a text resource *asynchronously*
 * and raise the given callbacks *synchronously*
 */
void LoadUrlAsync_Text(
  const std::string& url,
  const OnSuccessFunction<std::string>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr
);

/**
 * @brief LoadUrlAsync_Text will load a binary resource *asynchronously*
 * and raise the given callbacks *synchronously*
 */
void LoadUrlAsync_Binary(
  const std::string& url,
  const OnSuccessFunction<ArrayBuffer>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr
);

/**
 * @brief HeartBeat_Sync: call this in the app's main loop:
 * it will run the first available callback *synchronously*
 * Don't forget to call this in the main loop!
 *
 * See demo `SampleApplicationLoop()` inside src/BabylonCpp/include/babylon/asio/asio.h
 */
void HeartBeat_Sync();


/**
 * @brief HasRemainingTasks: returns true if io downloads or callbacks are still pending
 */
bool HasRemainingTasks();


/**
 * @brief Service_WaitAll_Sync: wait synchronously for all tasks completion
 * (should be used only in testing code)
 */
void Service_WaitAll_Sync();

} // namespace asio
} // namespace BABYLON

#endif // #ifndef BABYLON_ASYNC_REQUESTS_H