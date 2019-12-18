#ifndef BABYLON_ASYNC_REQUESTS_H
#define BABYLON_ASYNC_REQUESTS_H

#include <babylon/babylon_common.h>

#include <variant>
#include <functional>
#include <string>

namespace BABYLON {
namespace asio {


using OnErrorFunction = std::function<void(const std::string& errorMessage)>;

struct ErrorMessage {
  std::string errorMessage;
  explicit ErrorMessage(const std::string & message): errorMessage(message) {};
};

template <typename DataType>
using DataTypeOrErrorMessage = std::variant<DataType, ErrorMessage>;

template <typename DataType>
using SyncLoaderFunction = std::function<DataTypeOrErrorMessage<DataType>()>;

DataTypeOrErrorMessage<std::string> LoadTextFileSync(const std::string& filename);

void LoadFileAsync_Text(
  const std::string& filename,
  const std::function<void(const std::string& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction);

void LoadFileAsync_Binary(
  const std::string& filename,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction);


void Service_WaitAll();

} // namespace asio
} // namespace BABYLON

#endif // #ifndef BABYLON_ASYNC_REQUESTS_H