#ifndef BABYLONCPP_SYNC_IO_TYPES_H
#define BABYLONCPP_SYNC_IO_TYPES_H

#include <babylon/babylon_common.h>
#include <string>
#include <variant>
#include <functional>

namespace BABYLON {
namespace asio {
namespace sync_io_impl
{
using ArrayBufferOrErrorMessage = std::variant<ArrayBuffer, ErrorMessage>;
using SyncLoaderFunction = std::function<ArrayBufferOrErrorMessage()>;

} // namespace internal
} // namespace asio
} // namespace BABYLON


#endif // BABYLONCPP_SYNC_IO_TYPES_H
