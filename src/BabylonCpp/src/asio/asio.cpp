#include <babylon/asio/asio.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/string.h>

#include <deque>
#include <future>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

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

template <typename DataType>
using DataTypeOrErrorMessage = std::variant<DataType, ErrorMessage>;

template <typename DataType>
using SyncLoaderFunction = std::function<DataTypeOrErrorMessage<DataType>()>;

template<typename DataType>
using OnSuccessFunction            = std::function<void(const DataType& data)>;

template<typename DataType>
using FutureDataTypeOrErrorMessage = std::future<DataTypeOrErrorMessage<DataType>>;

using VoidCallback = std::function<void()>;


void EmptyVoidCallback() {}

template<typename DataType>
struct FutureAndCallbacks {
  OnSuccessFunction<DataType> onSuccessFunction;
  OnErrorFunction onErrorFunction;
  FutureDataTypeOrErrorMessage<DataType> futureDataTypeOrErrorMessage;


  std::optional<VoidCallback> shallCallNextCallback()
  {
    auto status = get_future_running_status(futureDataTypeOrErrorMessage);
    if (status != future_running_status::future_ready)
      return std::nullopt;

    DataTypeOrErrorMessage<DataType> v = futureDataTypeOrErrorMessage.get();

    VoidCallback nextCallback = EmptyVoidCallback;
    if (std::holds_alternative<ErrorMessage>(v))
    {
      if (onErrorFunction)
      {
        const auto& errorMessage = std::get<ErrorMessage>(v);

        auto onErrorFunctionCopy = this->onErrorFunction;
        nextCallback = [errorMessage, onErrorFunctionCopy]() {
          onErrorFunctionCopy(errorMessage.errorMessage);
        };
      }
    }

    if (std::holds_alternative<DataType>(v))
    {
      if (onSuccessFunction)
      {
        const auto& data = std::get<DataType>(v);

        auto onSuccessFunctionCopy = this->onSuccessFunction;
        nextCallback = [onSuccessFunctionCopy, data]() {
          onSuccessFunctionCopy(data);
        };
      }
    }
    return nextCallback;
  }

};


template <typename DataType>
class AsyncLoadService {
private:
  AsyncLoadService()
  {
    mStopRequested       = false;
    mHasRunningIOTasks       = false;
    mCheckIOCompletion_Async = really_async([this]() { this->CheckIOCompletion_AsyncProc(); });
  }
  ~AsyncLoadService()
  {
    mStopRequested = true;
  }

  void CheckTasksStatus()
  {
    std::lock_guard<std::mutex> guard(mMutexRunningIOTasks);
    std::vector<FutureAndCallbacks<DataType>> stillRunningTasks;
    for (auto& runningTask : mRunningIOTasks)
    {
      std::optional<VoidCallback> nextCallback = runningTask.shallCallNextCallback();
      if (nextCallback.has_value())
        mRemainingCallbacksSync.emplace_back(std::move(nextCallback.value()));
      else
        stillRunningTasks.emplace_back(std::move(runningTask));
    }
    mRunningIOTasks  = std::move(stillRunningTasks);
    mHasRunningIOTasks = !mRunningIOTasks.empty();
  }

  void CheckIOCompletion_AsyncProc() // This will be called in a parallel thread
  {
    using namespace std::literals;
    while (!mStopRequested) {
      CheckTasksStatus();
      std::this_thread::sleep_for(50ms);
    }
  }

public:
  void LoadData(
    const SyncLoaderFunction<DataType>& syncLoader,
    const OnSuccessFunction<DataType>& onSuccessFunction,
    const OnErrorFunction& onErrorFunction
  )
  {
    FutureDataTypeOrErrorMessage<DataType> futureData = really_async(syncLoader);
    FutureAndCallbacks<DataType> payload{onSuccessFunction, onErrorFunction, std::move(futureData)};
    mHasRunningIOTasks = true;

    std::lock_guard<std::mutex> guard(mMutexRunningIOTasks);
    mRunningIOTasks.emplace_back(std::move(payload));
  }

  static AsyncLoadService<DataType>& Instance()
  {
    static AsyncLoadService<DataType> instance;
    return instance;
  }

  void HeartBeat_Sync()
  {
    if (!mRemainingCallbacksSync.empty())
    {
      auto callback = mRemainingCallbacksSync.front();
      mRemainingCallbacksSync.pop_front();
      callback();
    }
  }

  void WaitAll_Sync()
  {
    using namespace std::literals;
    while(HasRemainingTasks())
    {
      HeartBeat_Sync();
      // In the meantime, mCheckIOCompletion_Async is still running
    }
  }

  bool HasRemainingTasks()
  {
    return mHasRunningIOTasks || !mRemainingCallbacksSync.empty();
  }

private:
  std::vector<FutureAndCallbacks<DataType>> mRunningIOTasks;
  std::atomic<bool> mHasRunningIOTasks;
  std::mutex mMutexRunningIOTasks;

  std::deque<VoidCallback> mRemainingCallbacksSync;

  std::future<void> mCheckIOCompletion_Async;
  std::atomic<bool> mStopRequested;

};


DataTypeOrErrorMessage<std::string> LoadFileSync_Text(
  const std::string& filename, const OnProgressFunction& onProgressFunction)
{
  std::ifstream ifs(filename);
  if (!ifs.good())
    return ErrorMessage("LoadFileSync_Text: Could not open file " + std::string(filename));

  std::string str;


  ifs.seekg(0, std::ios::end);
  size_t fileSize = ifs.tellg();
  str.reserve(fileSize);
  if (onProgressFunction)
    onProgressFunction(true, 0, fileSize);
  ifs.seekg(0, std::ios::beg);
  str.assign((std::istreambuf_iterator<char>(ifs)),
             std::istreambuf_iterator<char>());
  if (onProgressFunction)
    onProgressFunction(true, fileSize, fileSize);
  return str;
}

DataTypeOrErrorMessage<ArrayBuffer> LoadFileSync_Binary(
  const std::string& filename,
  const OnProgressFunction& onProgressFunction
)
{
  std::ifstream ifs(filename.c_str(), std::ios::binary | std::ios::ate);
  if (!ifs.good())
    return ErrorMessage("LoadFileSync_Binary: Could not open file " + std::string(filename));

  size_t fileSize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  ArrayBuffer buffer;
  buffer.resize(fileSize);

  size_t alreadyReadSize = 0;
  size_t blockSize = 1024 * 1024; // blocks of 1MB

  while (alreadyReadSize < fileSize)
  {
    if (onProgressFunction)
      onProgressFunction(true, alreadyReadSize, fileSize);

    size_t sizeToRead = fileSize - alreadyReadSize > blockSize ? blockSize : fileSize - alreadyReadSize;
    char * bufferPosition = (char*)(buffer.data() + alreadyReadSize);
    ifs.read(bufferPosition, sizeToRead);
    alreadyReadSize += sizeToRead;
  }

  return buffer;
}

void LoadFileAsync_Text(const std::string& filename,
                       const std::function<void(const std::string& data)>& onSuccessFunction,
                       const OnErrorFunction& onErrorFunction,
                       const OnProgressFunction& onProgressFunction
                       )
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto syncLoader = [filename, onProgressFunction]() {
    return LoadFileSync_Text(filename, onProgressFunction);
  };
  service.LoadData(syncLoader, onSuccessFunction, onErrorFunction);
}

void LoadFileAsync_Binary(
  const std::string& filename,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction
  )
{
  auto & service = AsyncLoadService<ArrayBuffer>::Instance();
  auto syncLoader = [filename, onProgressFunction]() {
    return LoadFileSync_Binary(filename, onProgressFunction);
  };
  service.LoadData(syncLoader, onSuccessFunction, onErrorFunction);
}

void LoadUrlAsync_Text(
  const std::string& url,
  const std::function<void(const std::string& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction
)
{
  if (!String::startsWith(url, "file:/"))
    throw std::runtime_error("LoadUrlAsync_Text only support files for the moment");
  std::string filename = url.substr(6);
  LoadFileAsync_Text(filename, onSuccessFunction, onErrorFunction, onProgressFunction);
}

void LoadUrlAsync_Binary(
  const std::string& url,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction,
  const OnProgressFunction& onProgressFunction
)
{
  if (!String::startsWith(url, "file:/"))
    throw std::runtime_error("LoadUrlAsync_Text only support files for the moment");
  std::string filename = url.substr(6);
  LoadFileAsync_Binary(filename, onSuccessFunction, onErrorFunction, onProgressFunction);
}

// Call this in the app's main loop: it will run the callbacks synchronously
// after the io completion
void HeartBeat_Sync()
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto & service2 = AsyncLoadService<BABYLON::ArrayBuffer>::Instance();
  service.HeartBeat_Sync();
  service2.HeartBeat_Sync();
}


void Service_WaitAll_Sync()
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto & service2 = AsyncLoadService<BABYLON::ArrayBuffer>::Instance();
  service.WaitAll_Sync();
  service2.WaitAll_Sync();
}

bool HasRemainingTasks()
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto & service2 = AsyncLoadService<BABYLON::ArrayBuffer>::Instance();
  return service.HasRemainingTasks() || service2.HasRemainingTasks();
}

} // namespace asio
} // namespace BABYLON