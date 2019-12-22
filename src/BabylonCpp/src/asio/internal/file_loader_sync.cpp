#include <babylon/asio/internal/file_loader_sync.h>
#include <babylon/asio/internal/sync_callback_runner.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/logging.h>
#include <fstream>

namespace BABYLON {
namespace asio {
namespace sync_io_impl
{

std::string tryFindFile(const std::string filename)
{
  // Check if the file is locally available
  // - Check in local folder
  auto absolutePath = Filesystem::absolutePath(filename);
  if (Filesystem::isFile(absolutePath)) {
    return absolutePath;
  }
  // - Check in assets folder
  absolutePath = Filesystem::absolutePath(BABYLON::assets_folder() + filename);
  if (Filesystem::isFile(absolutePath)) {
    return absolutePath;
  }

  // failure
  return filename;
}

ArrayBufferOrErrorMessage LoadFileSync_Binary(
  const std::string& filenameNotSearched,
  const OnProgressFunction& onProgressFunction
)
{
  std::string filename = tryFindFile(filenameNotSearched);

  std::ifstream ifs(filename.c_str(), std::ios::binary | std::ios::ate);
  if (!ifs.good()) {
    std::string message = "LoadFileSync_Binary: Could not open file " + std::string(filename);
    return ErrorMessage(message);
  }

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

  BABYLON_LOG_DEBUG("LoadFileSync_Binary", "Finished loading ", filename.c_str());
  return buffer;
}


} // namespace internal
} // namespace asio
} // namespace BABYLON
