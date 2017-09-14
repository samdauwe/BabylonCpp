#ifndef BABYLON_MATERIALS_COLOR_CURVES_H
#define BABYLON_MATERIALS_COLOR_CURVES_H

#include <babylon/babylon_global.h>
#include <babylon/math/color4.h>

namespace BABYLON {

/**
 * @brief The color grading curves provide additional color adjustmnent that is
 * applied after any color grading transform (3D LUT). They allow basic
 * adjustment of saturation and small exposure adjustments, along with color
 * filter tinting to provide white balance adjustment or more stylistic effects.
 * These are similar to controls found in many professional imaging or colorist
 * software. The global controls are applied to the entire image. For advanced
 * tuning, extra controls are provided to adjust the shadow, midtone and
 * highlight areas of the image; corresponding to low luminance, medium
 * luminance, and high luminance areas respectively.
 */
class BABYLON_SHARED_EXPORT ColorCurves {

public:
  ColorCurves();
  ColorCurves(const ColorCurves& other);            // Copy constructor
  ColorCurves(ColorCurves&& other);                 // Move constructor
  ColorCurves& operator=(const ColorCurves& other); // Copy assignment operator
  ColorCurves& operator=(ColorCurves&& other);      // Move assignment operator
  ~ColorCurves();

  /**
   * @brief Returns the string "ColorCurves".
   */
  const char* getClassName() const;

  /**
   * @brief Clones the current color curve instance.
   * @return The cloned curves
   */
  std::unique_ptr<ColorCurves> clone() const;

  /**
   * @brief Serializes the current color curve instance to a json
   * representation.
   * @return a JSON representation
   */
  Json::object serialize() const;

  /**
   * @brief Gets the global Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  float globalHue() const;

  /**
   * @brief Sets the global Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  void setGlobalHue(float value);

  /**
   * @brief Gets the global Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  float globalDensity() const;

  /**
   * @brief Sets the global Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  void setGlobalDensity(float value);

  /**
   * @brief Gets the global Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  float globalSaturation() const;

  /**
   * @brief Sets the global Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  void setGlobalSaturation(float value);

  /**
   * @brief Gets the highlights Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  float highlightsHue() const;

  /**
   * @brief Sets the highlights Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  void setHighlightsHue(float value);

  /**
   * @brief Gets the highlights Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  float highlightsDensity() const;

  /**
   * @brief Sets the highlights Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  void setHighlightsDensity(float value);

  /**
   * @brief Gets the highlights Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  float highlightsSaturation() const;

  /**
   * @brief Sets the highlights Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  void setHighlightsSaturation(float value);

  /**
   * @brief Gets the highlights Exposure value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase exposure and
   * negative values decrease exposure.
   */
  float highlightsExposure() const;

  /**
   * @brief Sets the highlights Exposure value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase exposure and
   * negative values decrease exposure.
   */
  void setHighlightsExposure(float value);

  /**
   * @brief Gets the midtones Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  float midtonesHue() const;

  /**
   * @brief Sets the midtones Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  void setMidtonesHue(float value);

  /**
   * @brief Gets the midtones Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  float midtonesDensity() const;

  /**
   * @brief Sets the midtones Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  void setMidtonesDensity(float value);

  /**
   * @brief Gets the midtones Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  float midtonesSaturation() const;

  /**
   * @brief Sets the midtones Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  void setMidtonesSaturation(float value);

  /**
   * @brief Gets the midtones Exposure value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase exposure and
   * negative values decrease exposure.
   */
  float midtonesExposure() const;

  /**
   * @brief Sets the midtones Exposure value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase exposure and
   * negative values decrease exposure.
   */
  void setMidtonesExposure(float value);

  /**
   * @brief Gets the shadows Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  float shadowsHue() const;

  /**
   * @brief Sets the shadows Hue value.
   * The hue value is a standard HSB hue in the range [0,360] where 0=red,
   * 120=green and 240=blue. The default value is 30 degrees (orange).
   */
  void setShadowsHue(float value);

  /**
   * @brief Gets the shadows Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  float shadowsDensity() const;

  /**
   * @brief Sets the shadows Density value.
   * The density value is in range [-100,+100] where 0 means the color filter
   * has no effect and +100 means the color filter has maximum effect.
   * Values less than zero provide a filter of opposite hue.
   */
  void setShadowsDensity(float value);

  /**
   * @brief Gets the shadows Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  float shadowsSaturation() const;

  /**
   * @brief Sets the shadows Saturation value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase saturation and
   * negative values decrease saturation.
   */
  void setShadowsSaturation(float value);

  /**
   * @brief Gets the shadows Exposure value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase exposure and
   * negative values decrease exposure.
   */
  float shadowsExposure() const;

  /**
   * @brief Sets the shadows Exposure value.
   * This is an adjustment value in the range [-100,+100], where the default
   * value of 0.0 makes no adjustment, positive values increase exposure and
   * negative values decrease exposure.
   */
  void setShadowsExposure(float value);

  /**
   * @brief Binds the color curves to the shader.
   * @param colorCurves The color curve to bind
   * @param effect The effect to bind to
   */
  static void
  Bind(ColorCurves& colorCurves, Effect* effect,
       const string_t& positiveUniform = "vCameraColorCurvePositive",
       const string_t& neutralUniform  = "vCameraColorCurveNeutral",
       const string_t& negativeUniform = "vCameraColorCurveNegative");

  /**
   * @brief Prepare the list of uniforms associated with the ColorCurves
   * effects.
   * @param uniformsList The list of uniforms used in the effect
   */
  static void PrepareUniforms(vector_t<string_t>& uniformsList);

  /**
   * @brief Returns color grading data based on a hue, density, saturation and
   * exposure value.
   * @param filterHue The hue of the color filter.
   * @param filterDensity The density of the color filter.
   * @param saturation The saturation.
   * @param exposure The exposure.
   * @param result The result data container.
   */
  void getColorGradingDataToRef(float hue, float density, float saturation,
                                float exposure, Color4& result);
  /**
   * @brief Takes an input slider value and returns an adjusted value that
   * provides extra control near the centre.
   * @param value The input slider value in range [-100,100].
   * @returns Adjusted value.
   */
  static float applyColorGradingSliderNonlinear(float value);

  /**
   * @brief Returns an RGBA Color4 based on Hue, Saturation and Brightness (also
   * referred to as value, HSV).
   * @param hue The hue (H) input.
   * @param saturation The saturation (S) input.
   * @param brightness The brightness (B) input.
   * @result An RGBA color represented as Vector4.
   */
  static void fromHSBToRef(float hue, float saturation, float brightness,
                           Color4& result);

  /**
   * @brief Returns a value clamped between min and max
   * @param value The value to clamp
   * @param min The minimum of value
   * @param max The maximum of value
   * @returns The clamped value.
   */
  static float clamp(float value, float min, float max);

  /**
   * @brief Parses the color curve from a json representation.
   * @param source the JSON source to parse
   * @return The parsed curves
   */
  static std::unique_ptr<ColorCurves> Parse(const Json::value& source);

private:
  bool _dirty;
  Color4 _tempColor;
  Color4 _globalCurve;
  Color4 _highlightsCurve;
  Color4 _midtonesCurve;
  Color4 _shadowsCurve;
  Color4 _positiveCurve;
  Color4 _negativeCurve;

  float _globalHue;
  float _globalDensity;
  float _globalSaturation;
  float _globalExposure;

  float _highlightsHue;
  float _highlightsDensity;
  float _highlightsSaturation;
  float _highlightsExposure;

  float _midtonesHue;
  float _midtonesDensity;
  float _midtonesSaturation;
  float _midtonesExposure;

  float _shadowsHue;
  float _shadowsDensity;
  float _shadowsSaturation;
  float _shadowsExposure;

}; // end of class ColorCurves

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_COLOR_CURVES_H
