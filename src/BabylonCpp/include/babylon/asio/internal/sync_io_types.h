#ifndef BABYLONCPP_SYNC_IO_TYPES_H
#define BABYLONCPP_SYNC_IO_TYPES_H

#include <variant>
#include <string>
#include <functional>

namespace BABYLON {
namespace asio {
namespace sync_io_impl
{
template <typename DataType>
using DataTypeOrErrorMessage = std::variant<DataType, ErrorMessage>;

template <typename DataType>
using SyncLoaderFunction = std::function<DataTypeOrErrorMessage<DataType>()>;

} // namespace internal
} // namespace asio
} // namespace BABYLON


#endif // BABYLONCPP_SYNC_IO_TYPES_H
