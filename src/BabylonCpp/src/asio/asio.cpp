#include <babylon/asio/asio.h>
#include <babylon/core/filesystem.h>

#include <future>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>

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
class AsyncLoadService {
private:
  AsyncLoadService()
  {
    mStopRequested       = false;
    mBackgoundWork_Async = really_async([this]() { this->BackgroundWork(); });
    std::cerr << "Construct AsyncLoadService\n";
  }
  ~AsyncLoadService()
  {
    WaitAll();
    mStopRequested = true;
    std::cerr << "Destruct AsyncLoadService\n";
  }
  using OnSuccessFunction            = std::function<void(const DataType& data)>;
  using FutureDataTypeOrErrorMessage = std::future<DataTypeOrErrorMessage<DataType>>;

  struct FutureAndCallbacks {
    OnSuccessFunction onSuccessFunction;
    OnErrorFunction onErrorFunction;
    FutureDataTypeOrErrorMessage futureDataTypeOrErrorMessage;

    bool signalCallbacksIfFinished()
    {
      auto status = get_future_running_status(futureDataTypeOrErrorMessage);
      if (status == future_running_status::future_ready) {
        DataTypeOrErrorMessage<DataType> v = futureDataTypeOrErrorMessage.get();
        if (std::holds_alternative<ErrorMessage>(v)) {
          const auto& errorMessage = std::get<ErrorMessage>(v);
          onErrorFunction(errorMessage.errorMessage);
        }
        if (std::holds_alternative<DataType>(v)) {
          const auto& data = std::get<DataType>(v);
          onSuccessFunction(data);
        }
        return true;
      }
      return false;
    }

    bool stillRunning()
    {
      bool finished = signalCallbacksIfFinished();
      return !finished;
    }
  };

  void CheckTasksStatus()
  {
    std::vector<FutureAndCallbacks> stillRunningTasks;
    for (auto& runningTask : mRunningTasks) {
      if (runningTask.stillRunning())
        stillRunningTasks.emplace_back(std::move(runningTask));
    }
    mRunningTasks = std::move(stillRunningTasks);
  }

  void BackgroundWork()
  {
    using namespace std::literals;
    while (!mStopRequested) {
      CheckTasksStatus();
      std::this_thread::sleep_for(50ms);
    }
  }

  std::vector<FutureAndCallbacks> mRunningTasks;
  std::future<void> mBackgoundWork_Async;
  std::atomic<bool> mStopRequested;

public:
  void LoadData(const SyncLoaderFunction<DataType>& syncLoader, const OnSuccessFunction& onSuccessFunction,
                const OnErrorFunction& onErrorFunction)
  {
    FutureDataTypeOrErrorMessage futureData = really_async(syncLoader);
    FutureAndCallbacks payload{onSuccessFunction, onErrorFunction, std::move(futureData)};
    mRunningTasks.emplace_back(std::move(payload));
  }

  static AsyncLoadService<DataType>& Instance()
  {
    static AsyncLoadService<DataType> instance;
    return instance;
  }

  void WaitAll()
  {
    using namespace std::literals;
    while(!mRunningTasks.empty())
      std::this_thread::sleep_for(50ms);
  }
};


DataTypeOrErrorMessage<std::string> LoadTextFileSync(const std::string& filename)
{
  std::ifstream ifs(filename);
  if (!ifs.good())
    return ErrorMessage("LoadTextFile: Could not open file " + std::string(filename));

  std::string str;

  ifs.seekg(0, std::ios::end);
  str.reserve(ifs.tellg());
  ifs.seekg(0, std::ios::beg);
  str.assign((std::istreambuf_iterator<char>(ifs)),
             std::istreambuf_iterator<char>());
  //std::cerr << "Finished LoadTextFileSync\n";
  return str;
}

void LoadFileAsync_Text(const std::string& filename,
                       const std::function<void(const std::string& data)>& onSuccessFunction,
                       const OnErrorFunction& onErrorFunction)
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto syncLoader = [filename]() {
    return LoadTextFileSync(filename);
  };
  service.LoadData(syncLoader, onSuccessFunction, onErrorFunction);
}

void LoadFileAsync_Binary(
  const std::string& filename,
  const std::function<void(const ArrayBuffer& data)>& onSuccessFunction,
  const OnErrorFunction& onErrorFunction)
{
  auto & service = AsyncLoadService<ArrayBuffer>::Instance();
  auto syncLoader = [filename]() {
    ArrayBuffer r = BABYLON::Filesystem::readBinaryFile(filename.c_str());
    return r;
  };
  service.LoadData(syncLoader, onSuccessFunction, onErrorFunction);
}


void Service_WaitAll()
{
  auto & service = AsyncLoadService<std::string>::Instance();
  auto & service2 = AsyncLoadService<BABYLON::ArrayBuffer>::Instance();
  service.WaitAll();
  service2.WaitAll();
}

//SyncLoaderFunction<std::string> LoadTextAsync(const std::string& url)
//{
//  SyncLoaderFunction<std::string> f;
//  return f;
//}

// auto & gAsyncLoadString = AsyncLoadService<std::string>::Instance();
// AsyncLoadService<ArrayBuffer> gAsyncLoadArrayBuffer;

//void LoadTextAsync(const std::string& url,
//                   const std::function<void(const std::string& data)>& onSuccessFunction,
//                   const OnErrorFunction& onErrorFunction)
//{
//}

} // namespace asio
} // namespace BABYLON