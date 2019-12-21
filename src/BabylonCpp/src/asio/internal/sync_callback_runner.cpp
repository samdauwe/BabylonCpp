#include <babylon/asio/internal/sync_callback_runner.h>
#include <future>
#include <deque>

namespace BABYLON {
namespace asio {

void EmptyVoidCallback()
{
}

namespace sync_callback_runner {

std::deque<VoidCallback> gPendingCallbacks;
std::mutex gMutexPendingCallbacks;


void PushCallback(const VoidCallback & function)
{
  std::lock_guard<std::mutex> guard(gMutexPendingCallbacks);
  gPendingCallbacks.push_back(function);
}

void HeartBeat()
{
  VoidCallback callback;

  {
    std::lock_guard<std::mutex> guard(gMutexPendingCallbacks);
    if (!gPendingCallbacks.empty())
    {
      callback = gPendingCallbacks.front();
      gPendingCallbacks.pop_front();
    }
  }

  if (callback)
    callback();
}

bool HasRemainingCallbacks()
{
  std::lock_guard<std::mutex> guard(gMutexPendingCallbacks);
  return ! gPendingCallbacks.empty();
}

void CallAllPendingCallbacks()
{
  while(HasRemainingCallbacks())
    HeartBeat();
}

} // namespace sync_callback_runner
} // namespace asio
} // namespace BABYLON
