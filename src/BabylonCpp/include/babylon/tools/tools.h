#ifndef BABYLON_TOOLS_TOOLS_H
#define BABYLON_TOOLS_TOOLS_H

#include <functional>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/tools/ivalue_gradient.h>

namespace BABYLON {

class Color4;
class Engine;
class ProgressEvent;

/**
 * @brief Class containing a set of static utilities functions.
 */
struct BABYLON_SHARED_EXPORT Tools {

  /**
   * Gets or sets a global variable indicating if fallback texture must be used
   * when a texture cannot be loaded.
   */
  static bool UseFallbackTexture;

  /**
   * Texture content used if a texture cannot loaded.
   */
  static std::string fallbackTexture;

  /**
   * @brief Read the content of a byte array at a specified coordinates (taking
   * in account wrapping).
   * @param u defines the coordinate on X axis
   * @param v defines the coordinate on Y axis
   * @param width defines the width of the source data
   * @param height defines the height of the source data
   * @param pixels defines the source byte array
   * @param color defines the output color
   */
  static void FetchToRef(int u, int v, int width, int height,
                         const Uint8Array& pixels, Color4& color);

  /**
   * @brief Interpolates between a and b via alpha.
   * @param a The lower value (returned when alpha = 0)
   * @param b The upper value (returned when alpha = 1)
   * @param alpha The interpolation-factor
   * @return The mixed value
   */
  static float Mix(float a, float b, float alpha);

  /**
   * @brief Function indicating if a number is an exponent of 2.
   * @param value defines the value to test
   * @returns true if the value is an exponent of 2
   */
  static bool IsExponentOfTwo(size_t value);

  /**
   * @brief Find the next highest power of two.
   * @param x Number to start search from.
   * @return Next highest power of two.
   */
  static int CeilingPOT(int x);

  /**
   * @brief Find the next lowest power of two.
   * @param x Number to start search from.
   * @return Next lowest power of two.
   */
  static int FloorPOT(int x);

  /**
   * @brief Find the nearest power of two.
   * @param x Number to start search from.
   * @return Next nearest power of two.
   */
  static int NearestPOT(int x);

  /**
   * @brief Get the closest exponent of two.
   * @param value defines the value to approximate
   * @param max defines the maximum value to return
   * @param mode defines how to define the closest value
   * @returns closest exponent of two of the given value
   */
  static int GetExponentOfTwo(int value, int max,
                              unsigned int mode
                              = EngineConstants::SCALEMODE_NEAREST);

  /**
   * @brief Extracts the filename from a path.
   * @param path defines the path to use
   * @returns the filename
   */
  static std::string GetFilename(const std::string& path);

  /**
   * @brief Extracts the "folder" part of a path (everything before the
   * filename).
   * @param uri The URI to extract the info from
   * @param returnUnchangedIfNoSlash Do not touch the URI if no slashes are
   * present
   * @returns The "folder" part of the path
   */
  static std::string GetFolderPath(const std::string& uri,
                                   bool returnUnchangedIfNoSlash = false);

  /**
   * @brief Convert an angle in radians to degrees.
   * @param angle defines the angle to convert
   * @returns the angle in degrees
   */
  static float ToDegrees(float angle);

  /**
   * @brief Convert an angle in degrees to radians.
   * @param angle defines the angle to convert
   * @returns the angle in radians
   */
  static float ToRadians(float angle);

  /**
   * @brief Extracts minimum and maximum values from a list of indexed
   * positions.
   * @param positions defines the positions to use
   * @param indices defines the indices to the positions
   * @param indexStart defines the start index
   * @param indexCount defines the end index
   * @param bias defines bias value to add to the result
   * @return minimum and maximum values
   */
  static MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                        const Uint32Array& indices,
                                        size_t indexStart, size_t indexCount,
                                        const std::optional<Vector2>& bias
                                        = std::nullopt);

  /**
   * @brief Extracts minimum and maximum values from a list of positions.
   * @param positions defines the positions to use
   * @param start defines the start index in the positions array
   * @param count defines the number of positions to handle
   * @param bias defines bias value to add to the result
   * @param stride defines the stride size to use (distance between two
   * positions in the positions array)
   * @return minimum and maximum values
   */
  static MinMax
  ExtractMinAndMax(const Float32Array& positions, size_t start, size_t count,
                   const std::optional<Vector2>& bias = std::nullopt,
                   std::optional<unsigned int> stride = std::nullopt);

  static MinMaxVector2 ExtractMinAndMaxVector2(
    const std::function<std::optional<Vector2>(std::size_t index)>& feeder,
    const std::optional<Vector2>& bias = std::nullopt);
  static Image CreateCheckerboardImage(unsigned int size = 256);
  static Image CreateNoiseImage(unsigned int size = 8);

  /**
   * @brief Removes unwanted characters from an url.
   * @param url defines the url to clean
   * @returns the cleaned url
   */
  static std::string CleanUrl(std::string url);

  static std::string DecodeURIComponent(const std::string& s);

  /**
   * @brief Gets or sets a function used to pre-process url before using them to
   * load assets.
   */
  static std::function<std::string(std::string url)> PreprocessUrl;

  /**
   * @brief Converts an ArrayBuffer to an image.
   * @param buffer the arraybuffer holding the image data
   * @return the decoded image
   */
  static Image ArrayBufferToImage(const ArrayBuffer& buffer,
                                  bool flipVertically = true);

  static void LoadImageFromUrl(
    std::string url, const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError,
    bool flipVertically = false);
  static void LoadImageFromBuffer(
    const std::variant<std::string, ArrayBuffer, Image>& input,
    const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError);

  /**
   * @brief Loads a file.
   * @param url url string, ArrayBuffer, or Blob to load
   * @param onSuccess callback called when the file successfully loads
   * @param onProgress callback called while file is loading (if the server
   * supports this mode)
   * @param database  database for caching
   * @param useArrayBuffer defines a boolean indicating that date must be
   * returned as ArrayBuffer
   * @param onError callback called when the file fails to load
   */
  static void LoadFile(
    std::string url,
    const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                             const std::string& responseURL)>& callback,
    const std::function<void(const ProgressEvent& event)>& progressCallBack
    = nullptr,
    bool useArrayBuffer = false,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError
    = nullptr);

  /**
   * @brief Loads a file.
   * @param fileToLoad defines the file to load
   * @param callback defines the callback to call when data is loaded
   * @param progressCallBack defines the callback to call during loading process
   * @param useArrayBuffer defines a boolean indicating that data must be
   * returned as an ArrayBuffer
   * @returns a file request object
   */
  static void ReadFile(
    std::string fileToLoad,
    const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                             const std::string& responseURL)>& callback,
    const std::function<void(const ProgressEvent& event)>& onProgress,
    bool useArrayBuffer);

  /**
   * @brief Checks if a given vector is inside a specific range.
   * @param v defines the vector to test
   * @param min defines the minimum range
   * @param max defines the maximum range
   */
  static void CheckExtends(Vector3& v, Vector3& min, Vector3& max);

  /**
   * @brief Implementation from
   * http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript/2117523#answer-2117523
   * Be aware Math.random() could cause collisions, but:
   * "All but 6 of the 128 bits of the ID are randomly generated, which means
   * that for any two ids, there's a 1 in 2^^122 (or 5.3x10^^36) chance they'll
   * collide"
   * @returns a pseudo random id
   */
  static std::string RandomId();

  /**
   * @brief Test if the given uri is a base64 string.
   * @param uri The uri to test
   * @return True if the uri is a base64 string or false otherwise
   */
  static bool IsBase64(const std::string& uri);

  /**
   * @brief Decode the given base64 uri.
   * @param uri The uri to decode
   * @return The decoded base64 data.
   */
  static ArrayBuffer DecodeBase64(const std::string& uri);

  /**
   * @brief Polyfill for setImmediate.
   * @param action defines the action to execute after the current execution
   * block
   */
  static void SetImmediate(const std::function<void()>& immediate);

  /**
   * @brief Dumps the current bound framebuffer.
   * @param width defines the rendering width
   * @param height defines the rendering height
   * @param engine defines the hosting engine
   * @param successCallback defines the callback triggered once the data are
   * available
   * @param mimeType defines the mime type of the result
   * @param fileName defines the filename to download. If present, the result
   * will automatically be downloaded
   */
  static void DumpFramebuffer(int width, int height, Engine* engine);

  static void StartPerformanceCounter(const std::string&)
  {
  }
  static void StartPerformanceCounter(const std::string&, bool)
  {
  }
  static void EndPerformanceCounter(const std::string&)
  {
  }
  static void EndPerformanceCounter(const std::string&, bool)
  {
  }
  static void ExitFullscreen()
  {
  }
  static void RequestFullscreen(ICanvas*)
  {
  }

  /**
   * @brief Gets the current gradient from an array of IValueGradient.
   * @param ratio defines the current ratio to get
   * @param gradients defines the array of IValueGradient
   * @param updateFunc defines the callback function used to get the final value
   * from the selected gradients
   */
  template <typename T>
  static void GetCurrentGradient(
    float ratio, const std::vector<T>& gradients,
    const std::function<void(T& current, T& next, float scale)>& updateFunc)
  {
    for (size_t gradientIndex = 0; gradientIndex < gradients.size() - 1;
         ++gradientIndex) {
      auto currentGradient = gradients[gradientIndex];
      auto nextGradient    = gradients[gradientIndex + 1];

      if (ratio >= currentGradient.gradient && ratio <= nextGradient.gradient) {
        const auto scale = (ratio - currentGradient.gradient)
                           / (nextGradient.gradient - currentGradient.gradient);
        updateFunc(currentGradient, nextGradient, scale);
        return;
      }
    }

    // Use last index if over
    auto lastGradient = gradients.back();
    updateFunc(lastGradient, lastGradient, 1.f);
  }

}; // end of struct Tools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_TOOLS_H
