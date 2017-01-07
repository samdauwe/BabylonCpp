#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_CURVES_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_CURVES_FX_H

namespace BABYLON {

extern const char* colorCurves;

const char* colorCurves
  = "const vec3 HDTVRec709_RGBLuminanceCoefficients = vec3(0.2126, 0.7152, 0.0722);\n"
    "\n"
    "vec3 applyColorCurves(vec3 original) {\n"
    "  vec3 result = original;\n"
    "\n"
    "  // Apply colour grading curves: luma-based adjustments for saturation, exposure and white balance (color filter)\n"
    "  // Note: the luma-based ramp is calibrated so that at 50% luma the midtone adjustment is full active, and the shadow/highlight \n"
    "  // adjustments are fully active by 16% and 83% luma, respectively.\n"
    "  float luma = dot(result.rgb, HDTVRec709_RGBLuminanceCoefficients);\n"
    "\n"
    "  vec2 curveMix = clamp(vec2(luma * 3.0 - 1.5, luma * -3.0 + 1.5), vec2(0.0, 0.0), vec2(1.0, 1.0));\n"
    "  vec4 colorCurve = vCameraColorCurveNeutral + curveMix.x * vCameraColorCurvePositive - curveMix.y * vCameraColorCurveNegative;\n"
    "\n"
    "  result.rgb *= colorCurve.rgb;\n"
    "  result.rgb = mix(vec3(luma, luma, luma), result.rgb, colorCurve.a);\n"
    "\n"
    "  return result;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_CURVES_FX_H
