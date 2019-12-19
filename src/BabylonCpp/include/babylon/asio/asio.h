#ifndef BABYLON_ASYNC_REQUESTS_H
#define BABYLON_ASYNC_REQUESTS_H

#define ASIO_SYNC_CALLBACKS

#include <babylon/babylon_common.h>
#include <babylon/asio/callback_types.h>
#include <variant>
#include <functional>
#include <string>

namespace BABYLON {
namespace asio {


void LoadFileAsync_Text(
  const std::string& filename,
  const OnSuccessFunction<std::string>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr
  );

void LoadFileAsync_Binary(
  const std::string& filename,
  const OnSuccessFunction<ArrayBuffer>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr
  );

void LoadUrlAsync_Text(
  const std::string& url,
  const std::function<void(const std::string& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr
);

void LoadUrlAsync_Binary(
  const std::string& url,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr
);

// Call this in the app's main loop:
// after the io completion, it will run the callbacks *synchronously*
void HeartBeat_Sync();

//
bool HasRemainingTasks();

//
void Service_WaitAll_Sync();

} // namespace asio
} // namespace BABYLON

#endif // #ifndef BABYLON_ASYNC_REQUESTS_H