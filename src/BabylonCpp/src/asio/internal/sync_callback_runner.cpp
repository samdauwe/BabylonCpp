#include <babylon/asio/internal/sync_callback_runner.h>
#include <babylon/core/logging.h>
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
  int nbRemainingCallback = -1;
  int nbCallbackAtStart = -1;

  bool done = false;
  while (!done)
  {
    VoidCallback callback;
    {
      std::lock_guard<std::mutex> guard(gMutexPendingCallbacks);

      if (nbCallbackAtStart < 0)
        nbCallbackAtStart = static_cast<int>(gPendingCallbacks.size());

      if (!gPendingCallbacks.empty())
      {
        callback = gPendingCallbacks.front();
        gPendingCallbacks.pop_front();
      }
      nbRemainingCallback = static_cast<int>(gPendingCallbacks.size());
    }

    if (callback) {
      BABYLON_LOG_DEBUG("sync_callback_runner", "Calling one callback, remaining ", nbRemainingCallback);
      callback();
    }
    else
      done = true;
   }

  if (nbRemainingCallback != nbCallbackAtStart) {
    char msg[1000];
    snprintf(msg, 1000, "HeartBeat end, called %i callbacks, remaining %i",
             nbCallbackAtStart - nbRemainingCallback,
             nbRemainingCallback);
    BABYLON_LOG_DEBUG("sync_callback_runner", msg, "");
  }
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
