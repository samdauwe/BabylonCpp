#ifndef BABYLONCPP_CALLBACK_TYPES_H
#define BABYLONCPP_CALLBACK_TYPES_H

#include <string>
#include <functional>

namespace BABYLON {
namespace asio {

using OnErrorFunction = std::function<void(const std::string& errorMessage)>;

using OnProgressFunction =
std::function<   void (bool lengthComputable, size_t loaded, size_t total)  >;

template<typename DataType>
using OnSuccessFunction = std::function<void(const DataType& data)>;


} // namespace asio
} // namespace BABYLON

#endif // BABYLONCPP_CALLBACK_TYPES_H
