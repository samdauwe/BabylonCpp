#ifdef __EMSCRIPTEN__

#include <babylon/asio/asio.h>
#include <babylon/core/logging.h>
#include <babylon/misc/string_tools.h>
#include <iostream>
#include <emscripten.h>
#include <unordered_map>
#include <thread>
#include <chrono>
#include <sstream>

#define BABYLON_LOG_TRACE_ON
#include <babylon/core/log_trace.h>

namespace BABYLON {
namespace asio {


namespace
{

static std::string ArrayBufferToString(const ArrayBuffer & dataUint8)
{
  std::string dataString;
  dataString.resize(dataUint8.size());
  for (size_t i = 0; i < dataUint8.size(); ++i)
    dataString[i] = static_cast<char>(dataUint8[i]);
  dataString = BABYLON::StringTools::replace(dataString, "\r\n", "\n");
  return dataString;
}

struct DownloadInfo
{
  std::string url;
  std::function<void(const ArrayBuffer& data)> onSuccessFunction;
  std::function<void(const std::string& message)> onErrorFunction;
};

using DownloadId = int;

std::unordered_map<DownloadId, DownloadInfo> gDownloadInfos;

DownloadId storeDownloadInfo(
  const std::string &url,
  std::function<void(const ArrayBuffer& data)> onSuccessFunction,
  std::function<void(const std::string& message)> onErrorFunction)
{
  TRACE_WHERE_VAR(url);
  static int id = 0;
  ++id;
  DownloadInfo info {url, onSuccessFunction, onErrorFunction};
  gDownloadInfos[id] = info;
  return id;
}

DownloadInfo consumeDownloadInfo(DownloadId id)
{
  if (gDownloadInfos.find(id) == gDownloadInfos.end()) {
    std::stringstream msg;
    msg << "consumeDownloadInfo bad id:" << id <<  " gDownloadInfos size=" << gDownloadInfos.size();
    BABYLON_LOG_ERROR("asio_emscripten", msg.str().c_str(), "");
    throw std::runtime_error(msg.str().c_str());
  }
  DownloadInfo r = gDownloadInfos.at(id);
  TRACE_WHERE_VAR(r.url);
  gDownloadInfos.erase(id);
  return r;
}

// See https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_async_wget_data
void babylon_emscripten_onLoad(void *arg_downloadId, void *bufferData, int bufferSize)
{
  uint8_t* bufferDataAsUint8 = static_cast<uint8_t *>(bufferData);
  ArrayBuffer arrayBuffer(bufferDataAsUint8, bufferDataAsUint8 + bufferSize); // this makes a copy

  DownloadInfo info = consumeDownloadInfo((DownloadId)arg_downloadId);
  BABYLON_LOG_DEBUG("babylon_emscripten_onLoad", info.url.c_str(), " Success!");
  info.onSuccessFunction(arrayBuffer);
}

void babylon_emscripten_onError(void *arg_downloadId)
{
  DownloadInfo info = consumeDownloadInfo((DownloadId)arg_downloadId);
  std::string errorMessage = std::string("Error while downloading ") + info.url;
  BABYLON_LOG_DEBUG("babylon_emscripten_onError", info.url.c_str(), " Failure!");
  info.onErrorFunction(errorMessage);
}

static std::string BaseUrl() {
  return "./emscripten_http_assets/";
}

} // anonymous namespace

void push_HACK_DISABLE_ASYNC()
{
  BABYLON_LOG_WARN("asio", "push_HACK_DISABLE_ASYNC does not work under emscripten", "");
}
void pop_HACK_DISABLE_ASYNC()
{
  BABYLON_LOG_WARN("asio", "pop_HACK_DISABLE_ASYNC does not work under emscripten", "");
}

void LoadUrlAsync_Text(
  const std::string& url,
  const std::function<void(const std::string& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction
)
{
  std::string fullUrl = BaseUrl() + url;
  auto onSuccessFunctionArrayBuffer = [onSuccessFunction](const ArrayBuffer& dataUint8) {
    onSuccessFunction(ArrayBufferToString(dataUint8));
  };
  auto downloadId = storeDownloadInfo(fullUrl.c_str(), onSuccessFunctionArrayBuffer, onErrorFunction);
  emscripten_async_wget_data(fullUrl.c_str(), (void*)downloadId, babylon_emscripten_onLoad, babylon_emscripten_onError);
}

void LoadUrlAsync_Binary(
  const std::string& url,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction
)
{
  std::string fullUrl = BaseUrl() + url;
  auto downloadId = storeDownloadInfo(fullUrl.c_str(), onSuccessFunction, onErrorFunction);
  emscripten_async_wget_data(fullUrl.c_str(), (void*)downloadId, babylon_emscripten_onLoad, babylon_emscripten_onError);
}

// Call this in the app's main loop: it will run the callbacks synchronously
// after the io completion
void HeartBeat_Sync()
{
}


void Service_WaitAll_Sync()
{
  using namespace std::literals;
  while(!gDownloadInfos.empty())
    std::this_thread::sleep_for(30ms);
}

BABYLON_SHARED_EXPORT void Service_Stop()
{
}

bool HasRemainingTasks()
{
  return !gDownloadInfos.empty();
}

} // namespace asio
} // namespace BABYLON

#endif //#ifdef __EMSCRIPTEN__
