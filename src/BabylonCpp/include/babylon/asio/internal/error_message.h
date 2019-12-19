#ifndef BABYLONCPP_ERROR_MESSAGE_H
#define BABYLONCPP_ERROR_MESSAGE_H

#include <string>

namespace BABYLON {
namespace asio {
namespace sync_io_impl {

struct ErrorMessage {
  std::string errorMessage;
  explicit ErrorMessage(const std::string & message): errorMessage(message) {};
};

} // namespace internal
} // namespace asio
} // namespace BABYLON


#endif // BABYLONCPP_ERROR_MESSAGE_H
