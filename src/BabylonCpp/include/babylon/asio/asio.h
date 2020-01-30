#ifndef BABYLON_ASYNC_REQUESTS_H
#define BABYLON_ASYNC_REQUESTS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/asio/callback_types.h>
#include <variant>
#include <functional>
#include <string>

namespace BABYLON {
namespace asio {



/**
 * @brief LoadAssetAsync_Text will load a text resource *asynchronously*
 * and raise the given callbacks *synchronously*
 */
BABYLON_SHARED_EXPORT void
LoadAssetAsync_Text(
  const std::string& assetPath, const OnSuccessFunction<std::string>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr);

/**
 * @brief LoadAssetAsync_Text will load a binary resource *asynchronously*
 * and raise the given callbacks *synchronously*
 */
BABYLON_SHARED_EXPORT void LoadAssetAsync_Binary(
  const std::string& assetPath,
  const OnSuccessFunction<ArrayBuffer>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr);

/**
 * @brief HeartBeat_Sync: call this in the app's main loop:
 * it will run the first available callback *synchronously*
 * Don't forget to call this in the main loop!
 *
 * See demo `SampleApplicationLoop()` inside src/BabylonCpp/include/babylon/asio/asio.h
 */
BABYLON_SHARED_EXPORT void HeartBeat_Sync();


/**
 * @brief HasRemainingTasks: returns true if io downloads or callbacks are still pending
 */
BABYLON_SHARED_EXPORT bool HasRemainingTasks();


/**
 * @brief Service_WaitAll_Sync: wait synchronously for all tasks completion
 * (should be used only in testing code)
 */
BABYLON_SHARED_EXPORT void Service_WaitAll_Sync();

BABYLON_SHARED_EXPORT void Service_Stop();

/**
 * Desesperate patch for glTF loading
 */
BABYLON_SHARED_EXPORT void push_HACK_DISABLE_ASYNC();
BABYLON_SHARED_EXPORT void pop_HACK_DISABLE_ASYNC();

} // namespace asio
} // namespace BABYLON

#endif // #ifndef BABYLON_ASYNC_REQUESTS_H