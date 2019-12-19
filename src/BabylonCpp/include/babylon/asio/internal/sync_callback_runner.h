#ifndef BABYLONCPP_SYNC_CALLBACK_RUNNER_H
#define BABYLONCPP_SYNC_CALLBACK_RUNNER_H

#include <functional>

namespace BABYLON {
namespace asio {

using VoidCallback = std::function<void()>;
void EmptyVoidCallback();

namespace sync_callback_runner {


void PushCallback(const VoidCallback & function);
void HeartBeat();
bool HasRemainingCallbacks();

void CallAllPendingCallbacks();

} // namespace sync_callback_runner
} // namespace asio
} // namespace BABYLON

#endif // BABYLONCPP_SYNC_CALLBACK_RUNNER_H
