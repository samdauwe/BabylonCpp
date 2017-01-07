#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_GRADING_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_GRADING_FX_H

namespace BABYLON {

extern const char* colorGrading;

const char* colorGrading
  = "vec4 colorGrades(vec4 color) \n"
    "{    \n"
    "  // Dynamic runtime calculations (dependent on input color)\n"
    "  float sliceContinuous = color.z * vCameraColorGradingInfos.z;\n"
    "  float sliceInteger = floor(sliceContinuous);\n"
    "\n"
    "  // Note: this is mathematically equivalent to fract(sliceContinuous); but we use explicit subtract\n"
    "  // rather than separate fract() for correct results near slice boundaries (matching sliceInteger choice)\n"
    "  float sliceFraction = sliceContinuous - sliceInteger; \n"
    "\n"
    "  // Calculate UV offset from slice origin (top-left)\n"
    "  vec2 sliceUV = color.xy * vCameraColorGradingScaleOffset.xy + vCameraColorGradingScaleOffset.zw;\n"
    "\n"
    "  // Calculate UV positions into overall texture for neighbouring slices \n"
    "  // (to emulate trilinear filtering on missing 3D hardware texture support)\n"
    "  sliceUV.x += sliceInteger * vCameraColorGradingInfos.w;\n"
    "  vec4 slice0Color = texture2D(cameraColorGrading2DSampler, sliceUV);\n"
    "\n"
    "  sliceUV.x += vCameraColorGradingInfos.w;\n"
    "  vec4 slice1Color = texture2D(cameraColorGrading2DSampler, sliceUV);\n"
    "\n"
    "  vec3 result = mix(slice0Color.rgb, slice1Color.rgb, sliceFraction);\n"
    "  color.rgb = mix(color.rgb, result, vCameraColorGradingInfos.x);\n"
    "\n"
    "  return color;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_GRADING_FX_H
