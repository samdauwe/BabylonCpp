#ifndef BABYLON_MISC_FILE_TOOLS_H
#define BABYLON_MISC_FILE_TOOLS_H

#include <functional>
#include <string>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>

namespace BABYLON {

struct ArrayBufferView;
class ProgressEvent;

/**
 * @brief Hidden.
 */
class BABYLON_SHARED_EXPORT FileTools {

public:
  /**
   * @brief Gets or sets a function used to pre-process url before using them to load assets.
   */
  static std::function<std::string(std::string url)> PreprocessUrl;

  /**
   * @brief Loads an image from an url.
   * @param input url string, ArrayBuffer, or Blob to load
   * @param onLoad callback called when the image successfully loads
   * @param onError callback called when the image fails to load
   * @param flipVertically whether or not to flip the image vertically
   */
  static void LoadImageFromUrl(
    std::string url, const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError,
    bool flipVertically = false);

  /**
   * @brief Loads an image from a buffer.
   * @param input url string, ArrayBuffer, or Blob to load
   * @param onLoad callback called when the image successfully loads
   * @param onError callback called when the image fails to load
   * @param flipVertically whether or not to flip the image vertically
   */
  static void LoadImageFromBuffer(
    const std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>& input, bool invertY,
    const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError);

  /**
   * @brief Loads a file.
   * @param fileToLoad defines the file to load
   * @param callback defines the callback to call when data is loaded
   * @param progressCallBack defines the callback to call during loading process
   * @param useArrayBuffer defines a boolean indicating that data must be returned as an ArrayBuffer
   */
  static void ReadFile(std::string fileToLoad,
                       const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                                                const std::string& responseURL)>& callback,
                       const std::function<void(const ProgressEvent& event)>& onProgress,
                       bool useArrayBuffer);

  /**
   * @brief Loads a file.
   * @param url url string, ArrayBuffer, or Blob to load
   * @param onSuccess callback called when the file successfully loads
   * @param onProgress callback called while file is loading (if the server supports this mode)
   * @param offlineProvider defines the offline provider for caching
   * @param useArrayBuffer defines a boolean indicating that date must be returned as ArrayBuffer
   * @param onError callback called when the file fails to load
   */
  static void LoadFile(
    std::string url,
    const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                             const std::string& responseURL)>& callback,
    const std::function<void(const ProgressEvent& event)>& progressCallBack = nullptr,
    bool useArrayBuffer                                                     = false,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);

  /** Helper functions */

  /**
   * @brief Converts an ArrayBuffer to an image.
   * @param buffer the arraybuffer holding the image data
   * @return the decoded image
   */
  static Image ArrayBufferToImage(const ArrayBuffer& buffer, bool flipVertically = false);

  /**
   * @brief Converts an string to an image.
   * @param buffer the string holding the image data
   * @return the decoded image
   */
  static Image StringToImage(const std::string& buffer, bool flipVertically = false);

private:
  /**
   * @brief Removes unwanted characters from an url.
   * @param url defines the url to clean
   * @returns the cleaned url
   */
  static std::string _CleanUrl(std::string url);

}; // end of class FileTools

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_FILE_TOOLS_H
