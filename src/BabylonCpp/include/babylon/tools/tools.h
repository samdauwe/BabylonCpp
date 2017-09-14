#ifndef BABYLON_TOOLS_TOOLS_H
#define BABYLON_TOOLS_TOOLS_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>
#include <babylon/core/structs.h>
#include <babylon/engine/engine_constants.h>

namespace BABYLON {

/**
 * @brief Represents the tools class.
 */
struct BABYLON_SHARED_EXPORT Tools {

  /** Statics **/

  /**
   * @brief Interpolates between a and b via alpha.
   * @param a The lower value (returned when alpha = 0)
   * @param b The upper value (returned when alpha = 1)
   * @param alpha The interpolation-factor
   * @return The mixed value
   */
  static float Mix(float a, float b, float alpha);
  static bool IsExponentOfTwo(int value);

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
  static float ToDegrees(float angle);
  static float ToRadians(float angle);
  static MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                        const Uint32Array& indices,
                                        size_t indexStart, size_t indexCount);
  static MinMax ExtractMinAndMaxIndexed(const Float32Array& positions,
                                        const Uint32Array& indices,
                                        size_t indexStart, size_t indexCount,
                                        const Vector2& bias);
  static MinMax ExtractMinAndMax(const Float32Array& positions, size_t start,
                                 size_t count, unsigned int stride = 3);
  static MinMax ExtractMinAndMax(const Float32Array& positions, size_t start,
                                 size_t count, const Vector2& bias,
                                 unsigned int stride = 3);
  static MinMaxVector2 ExtractMinAndMaxVector2(
    const ::std::function<Nullable<Vector2>(std::size_t index)>& feeder);
  static MinMaxVector2 ExtractMinAndMaxVector2(
    const ::std::function<Nullable<Vector2>(std::size_t index)>& feeder,
    const Vector2& bias);
  static Image CreateCheckerboardImage(unsigned int size = 256);
  static Image CreateNoiseImage(unsigned int size = 8);
  static void
  LoadImage(const string_t& url,
            const ::std::function<void(const Image& img)>& onLoad,
            const ::std::function<void(const string_t& msg)>& onError,
            bool flipVertically = true);
  static void
  LoadFile(const string_t& url,
           const ::std::function<void(const string_t& text)>& callback,
           const ::std::function<void()>& progressCallBack = nullptr,
           bool useArrayBuffer                             = false);
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

}; // end of struct Tools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_TOOLS_H
