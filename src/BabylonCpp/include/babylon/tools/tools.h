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
 * @brief Represents the tools class.
 */
struct BABYLON_SHARED_EXPORT Tools {

  /** Statics **/
  static bool UseFallbackTexture;

  // Used in case of a texture loading problem
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
   * Find the nearest power of two.
   * @param x Number to start search from.
   * @return Next nearest power of two.
   */
  static int NearestPOT(int x);

  static int GetExponentOfTwo(int value, int max,
                              unsigned int mode
                              = EngineConstants::SCALEMODE_NEAREST);
  static std::string GetFilename(const std::string& path);
  static std::string GetFolderPath(const std::string& uri,
                                   bool returnUnchangedIfNoSlash = false);
  static float ToDegrees(float angle);
  static float ToRadians(float angle);
  static MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                        const Uint32Array& indices,
                                        size_t indexStart, size_t indexCount,
                                        const std::optional<Vector2>& bias
                                        = std::nullopt);
  static MinMax
  ExtractMinAndMax(const Float32Array& positions, size_t start, size_t count,
                   const std::optional<Vector2>& bias = std::nullopt,
                   std::optional<unsigned int> stride = std::nullopt);
  static MinMaxVector2 ExtractMinAndMaxVector2(
    const std::function<std::optional<Vector2>(std::size_t index)>& feeder,
    const std::optional<Vector2>& bias = std::nullopt);
  static Image CreateCheckerboardImage(unsigned int size = 256);
  static Image CreateNoiseImage(unsigned int size = 8);
  // External files
  static std::string CleanUrl(std::string url);
  static std::string DecodeURIComponent(const std::string& s);
  static std::function<std::string(std::string url)> PreprocessUrl;
  static void LoadImageFromUrl(
    std::string url, const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError,
    bool flipVertically = true);
  static void LoadImageFromBuffer(
    const std::variant<std::string, ArrayBuffer, Image>& input,
    const std::function<void(const Image& img)>& onLoad,
    const std::function<void(const std::string& message,
                             const std::string& exception)>& onError);
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
  static void ReadFile(
    std::string fileToLoad,
    const std::function<void(const std::variant<std::string, ArrayBuffer>& data,
                             const std::string& responseURL)>& callback,
    const std::function<void(const ProgressEvent& event)>& onProgress,
    bool useArrayBuffer);
  static void CheckExtends(Vector3& v, Vector3& min, Vector3& max);
  static std::string RandomId();
  static void SetImmediate(const std::function<void()>& immediate);
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
      }
    }
  }

}; // end of struct Tools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_TOOLS_H
