#include <babylon/asio/asio.h>

#include <babylon/asio/internal/file_loader_sync.h>
#include <babylon/asio/internal/future_utils.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/string.h>
#include <babylon/asio/internal/sync_callback_runner.h>

#include <deque>
#include <future>
#include <atomic>
#include <chrono>

namespace BABYLON {
namespace asio {

using namespace sync_io_impl;

template<typename DataType>
using OnSuccessFunction            = std::function<void(const DataType& data)>;

template<typename DataType>
using FutureDataTypeOrErrorMessage = std::future<DataTypeOrErrorMessage<DataType>>;


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
        sync_callback_runner::PushCallback(std::move(nextCallback.value()));
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
      std::this_thread::sleep_for(15ms);
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

  void WaitIoCompletion_Sync()
  {
    using namespace std::literals;
    while(mHasRunningIOTasks)
      std::this_thread::sleep_for(50ms);
  }

  bool HasRunningIOTasks()
  {
    return mHasRunningIOTasks;
  }

private:
  std::vector<FutureAndCallbacks<DataType>> mRunningIOTasks;
  std::atomic<bool> mHasRunningIOTasks;
  std::mutex mMutexRunningIOTasks;

  std::future<void> mCheckIOCompletion_Async;
  std::atomic<bool> mStopRequested;

};



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
  sync_callback_runner::HeartBeat();
}


void Service_WaitAll_Sync()
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto & service2 = AsyncLoadService<BABYLON::ArrayBuffer>::Instance();
  service.WaitIoCompletion_Sync();
  service2.WaitIoCompletion_Sync();
  sync_callback_runner::CallAllPendingCallbacks();
}

bool HasRemainingTasks()
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto & service2 = AsyncLoadService<BABYLON::ArrayBuffer>::Instance();
  return    service.HasRunningIOTasks()
         || service2.HasRunningIOTasks()
         || sync_callback_runner::HasRemainingCallbacks();
}

} // namespace asio
} // namespace BABYLON