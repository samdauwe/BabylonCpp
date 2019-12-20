#include <babylon/asio/internal/file_loader_sync.h>
#include <babylon/asio/internal/sync_callback_runner.h>

#include <fstream>

namespace BABYLON {
namespace asio {
namespace sync_io_impl
{

ArrayBufferOrErrorMessage LoadFileSync_Binary(
  const std::string& filename,
  const OnProgressFunction& onProgressFunction
)
{
  std::ifstream ifs(filename.c_str(), std::ios::binary | std::ios::ate);
  if (!ifs.good())
    return ErrorMessage("LoadFileSync_Binary: Could not open file " + std::string(filename));

  size_t fileSize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  ArrayBuffer buffer;
  buffer.resize(fileSize);

  size_t alreadyReadSize = 0;
  size_t blockSize = 1024 * 1024; // blocks of 1MB

  while (alreadyReadSize < fileSize)
  {
    if (onProgressFunction)
    {
      auto f = [onProgressFunction, alreadyReadSize, fileSize]() {
        onProgressFunction(true, alreadyReadSize, fileSize);
      };
      sync_callback_runner::PushCallback(f);
    }

    size_t sizeToRead = fileSize - alreadyReadSize > blockSize ? blockSize : fileSize - alreadyReadSize;
    char * bufferPosition = (char*)(buffer.data() + alreadyReadSize);
    ifs.read(bufferPosition, sizeToRead);
    alreadyReadSize += sizeToRead;
  }

  if (onProgressFunction)
  {
    auto f = [onProgressFunction, alreadyReadSize, fileSize]() {
      onProgressFunction(true, alreadyReadSize, fileSize);
    };
    sync_callback_runner::PushCallback(f);
  }

  return buffer;
}


} // namespace internal
} // namespace asio
} // namespace BABYLON
