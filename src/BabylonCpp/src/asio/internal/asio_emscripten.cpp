#ifdef __EMSCRIPTEN__

#include <babylon/asio/asio.h>
#include <babylon/core/logging.h>
#include <babylon/core/string.h>
#include <iostream>
#include <emscripten.h>
#include <unordered_map>
#include <thread>
#include <chrono>

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
  dataString = BABYLON::String::replace(dataString, "\r\n", "\n");
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
  std::cout << "storeDownloadInfo 1 \n";
  static int id = 0;
  ++id;
  DownloadInfo info {url, onSuccessFunction, onErrorFunction};
  gDownloadInfos[id] = info;
  std::cout << "storeDownloadInfo 2 returned id " << id << " gDownloadInfos size=" << gDownloadInfos.size() << "\n";
  return id;
}

DownloadInfo consumeDownloadInfo(DownloadId id)
{
  std::cout << "consumeDownloadInfo 1 \n";
  if (gDownloadInfos.find(id) == gDownloadInfos.end()) {
    std::cout << "consumeDownloadInfo; bad id !!! -> " << id << "\n";
    throw std::runtime_error("consumeDownloadInfo; bad id");
  }
  std::cout << "consumeDownloadInfo 2 \n";
  DownloadInfo r = gDownloadInfos.at(id);
  gDownloadInfos.erase(id);
  std::cout << "consumeDownloadInfo 3 gDownloadInfos size=" << gDownloadInfos.size() << "\n";
  return r;
}

// See https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_async_wget_data
void babylon_emscripten_onLoad(void *arg_downloadId, void *bufferData, int bufferSize)
{
  std::cout << "babylon_emscripten_onLoad 1 bufferSize=" << bufferSize <<  "arg_downloadId=" << (int)arg_downloadId << "\n";
  uint8_t* bufferDataAsUint8 = static_cast<uint8_t *>(bufferData);
  ArrayBuffer arrayBuffer(bufferDataAsUint8, bufferDataAsUint8 + bufferSize); // this makes a copy
  std::cout << "babylon_emscripten_onLoad 2 \n";

  DownloadInfo info = consumeDownloadInfo((DownloadId)arg_downloadId);
  std::cout << "babylon_emscripten_onLoad 3 \n";
  info.onSuccessFunction(arrayBuffer);
  std::cout << "babylon_emscripten_onLoad 4 \n";
}

void babylon_emscripten_onError(void *arg_downloadId)
{
  std::cout << "babylon_emscripten_onError 1 " <<  "arg_downloadId=" << (int)arg_downloadId << "\n";
  DownloadInfo info = consumeDownloadInfo((DownloadId)arg_downloadId);
  std::cout << "babylon_emscripten_onError 2 \n";
  std::string errorMessage = std::string("Error while downloading ") + info.url;
  info.onErrorFunction(errorMessage);
  std::cout << "babylon_emscripten_onError 3 \n";
}

static std::string BaseUrl() {
  return "../assets/";
}

} // anonymous namespace


void set_HACK_DISABLE_ASYNC(bool v)
{
  BABYLON_LOG_WARN("asio", "set_HACK_DISABLE_ASYNC does not work under emscripten", "");
}

void LoadUrlAsync_Text(
  const std::string& url,
  const std::function<void(const std::string& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction
)
{
  std::cout << "LoadUrlAsync_Text 1 \n";
  std::string fullUrl = BaseUrl() + url;
  auto onSuccessFunctionArrayBuffer = [onSuccessFunction](const ArrayBuffer& dataUint8) {
    onSuccessFunction(ArrayBufferToString(dataUint8));
  };
  auto downloadId = storeDownloadInfo(fullUrl.c_str(), onSuccessFunctionArrayBuffer, onErrorFunction);
  std::cout << "LoadUrlAsync_Text 2 \n";
  emscripten_async_wget_data(fullUrl.c_str(), (void*)downloadId, babylon_emscripten_onLoad, babylon_emscripten_onError);
  std::cout << "LoadUrlAsync_Text 3 \n";
}

void LoadUrlAsync_Binary(
  const std::string& url,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction
)
{
  std::cout << "LoadUrlAsync_Binary 1 \n";
  std::string fullUrl = BaseUrl() + url;
  auto downloadId = storeDownloadInfo(fullUrl.c_str(), onSuccessFunction, onErrorFunction);
  std::cout << "LoadUrlAsync_Binary 2 \n";
  emscripten_async_wget_data(fullUrl.c_str(), (void*)downloadId, babylon_emscripten_onLoad, babylon_emscripten_onError);
  std::cout << "LoadUrlAsync_Binary 3 \n";
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
