#ifndef BABYLON_TOOLS_TOOLS_H
#define BABYLON_TOOLS_TOOLS_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/tools/ivalue_gradient.h>

namespace BABYLON {

/**
 * @brief Represents the tools class.
 */
struct BABYLON_SHARED_EXPORT Tools {

  /** Statics **/

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
  static string_t GetFilename(const string_t& path);
  static string_t GetFolderPath(const string_t& uri,
                                bool returnUnchangedIfNoSlash = false);
  static float ToDegrees(float angle);
  static float ToRadians(float angle);
  static MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                        const Uint32Array& indices,
                                        size_t indexStart, size_t indexCount,
                                        const nullable_t<Vector2>& bias
                                        = nullopt_t);
  static MinMax ExtractMinAndMax(const Float32Array& positions, size_t start,
                                 size_t count,
                                 const nullable_t<Vector2>& bias = nullopt_t,
                                 nullable_t<unsigned int> stride = nullopt_t);
  static MinMaxVector2 ExtractMinAndMaxVector2(
    const ::std::function<nullable_t<Vector2>(std::size_t index)>& feeder,
    const nullable_t<Vector2>& bias = nullopt_t);
  static Image CreateCheckerboardImage(unsigned int size = 256);
  static Image CreateNoiseImage(unsigned int size = 8);
  // External files
  static string_t CleanUrl(string_t url);
  static string_t DecodeURIComponent(const string_t& s);
  static ::std::function<string_t(string_t url)> PreprocessUrl;
  static void
  LoadImage(string_t url, const ::std::function<void(const Image& img)>& onLoad,
            const ::std::function<void(const string_t& msg)>& onError,
            bool flipVertically = true);
  static void LoadFile(
    string_t url,
    const ::std::function<void(const string_t& data,
                               const string_t& responseURL)>& callback,
    const ::std::function<void(const ProgressEvent& event)>& progressCallBack
    = nullptr,
    bool useArrayBuffer                                             = false,
    const ::std::function<void(const string_t& exception)>& onError = nullptr);
  static void
  ReadFile(string_t fileToLoad,
           const ::std::function<void(const string_t& data,
                                      const string_t& responseURL)>& callback,
           const ::std::function<void(const ProgressEvent& event)>& onProgress,
           bool useArrayBuffer);
  static void CheckExtends(Vector3& v, Vector3& min, Vector3& max);
  static string_t RandomId();
  static void SetImmediate(const ::std::function<void()>& immediate);
  static void DumpFramebuffer(int width, int height, Engine* engine);
  static void StartPerformanceCounter(const string_t&)
  {
  }
  static void StartPerformanceCounter(const string_t&, bool)
  {
  }
  static void EndPerformanceCounter(const string_t&)
  {
  }
  static void EndPerformanceCounter(const string_t&, bool)
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
    float ratio, const vector_t<T>& gradients,
    const ::std::function<void(T& current, T& next, float scale)>& updateFunc)
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
