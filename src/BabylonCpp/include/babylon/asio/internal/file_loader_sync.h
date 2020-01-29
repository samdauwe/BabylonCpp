//
// Created by Pascal Thomet on 19/12/2019.
//

#ifndef BABYLONCPP_FILE_LOADER_SYNC_H
#define BABYLONCPP_FILE_LOADER_SYNC_H

#include <babylon/asio/internal/error_message.h>
#include <babylon/asio/callback_types.h>
#include <babylon/babylon_common.h>
#include <babylon/asio/internal/sync_io_types.h>

namespace BABYLON {
namespace asio {
namespace sync_io_impl
{
ArrayBufferOrErrorMessage LoadFileSync_Binary(
  const std::string& filenameRelativeToAssetsDir,
  const OnProgressFunction& onProgressFunction
  );


} // namespace internal
} // namespace asio
} // namespace BABYLON


#endif // BABYLONCPP_FILE_LOADER_SYNC_H
