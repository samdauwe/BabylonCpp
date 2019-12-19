#ifndef BABYLON_ASYNC_REQUESTS_H
#define BABYLON_ASYNC_REQUESTS_H

#include <babylon/babylon_common.h>
#include <variant>
#include <functional>
#include <string>

namespace BABYLON {
namespace asio {


using OnErrorFunction = std::function<void(const std::string& errorMessage)>;

using OnProgressFunction =
  std::function<   void (bool lengthComputable, size_t loaded, size_t total)  >;

struct ErrorMessage {
  std::string errorMessage;
  explicit ErrorMessage(const std::string & message): errorMessage(message) {};
};

template <typename DataType>
using DataTypeOrErrorMessage = std::variant<DataType, ErrorMessage>;

template <typename DataType>
using SyncLoaderFunction = std::function<DataTypeOrErrorMessage<DataType>()>;

DataTypeOrErrorMessage<std::string> LoadFileSync_Text(
  const std::string& filename,
  const OnProgressFunction& onProgressFunction
  );

DataTypeOrErrorMessage<ArrayBuffer> LoadFileSync_Binary(
  const std::string& filename,
  const OnProgressFunction& onProgressFunction
  );

void LoadFileAsync_Text(
  const std::string& filename,
  const std::function<void(const std::string& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction = nullptr
  );

void LoadFileAsync_Binary(
  const std::string& filename,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
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


void Service_WaitAll();

} // namespace asio
} // namespace BABYLON

#endif // #ifndef BABYLON_ASYNC_REQUESTS_H