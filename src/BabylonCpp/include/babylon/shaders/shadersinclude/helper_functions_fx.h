#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_HELPER_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_HELPER_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* helperFunctions;

const char* helperFunctions
  = "const float PI = 3.1415926535897932384626433832795;\n"
    "\n"
    "const float LinearEncodePowerApprox = 2.2;\n"
    "const float GammaEncodePowerApprox = 1.0 / LinearEncodePowerApprox;\n"
    "const vec3 LuminanceEncodeApprox = vec3(0.2126, 0.7152, 0.0722);\n"
    "\n"
    "mat3 transposeMat3(mat3 inMatrix) {\n"
    "  vec3 i0 = inMatrix[0];\n"
    "  vec3 i1 = inMatrix[1];\n"
    "  vec3 i2 = inMatrix[2];\n"
    "\n"
    "  mat3 outMatrix = mat3(\n"
    "  vec3(i0.x, i1.x, i2.x),\n"
    "  vec3(i0.y, i1.y, i2.y),\n"
    "  vec3(i0.z, i1.z, i2.z)\n"
    "  );\n"
    "\n"
    "  return outMatrix;\n"
    "}\n"
    "\n"
    "float computeFallOff(float value, vec2 clipSpace, float frustumEdgeFalloff)\n"
    "{\n"
    "  float mask = smoothstep(1.0 - frustumEdgeFalloff, 1.0, clamp(dot(clipSpace, clipSpace), 0., 1.));\n"
    "  return mix(value, 1.0, mask);\n"
    "}\n"
    "\n"
    "vec3 applyEaseInOut(vec3 x){\n"
    "  return x * x * (3.0 - 2.0 * x);\n"
    "}\n"
    "\n"
    "vec3 toLinearSpace(vec3 color)\n"
    "{\n"
    "  return pow(color, vec3(LinearEncodePowerApprox));\n"
    "}\n"
    "\n"
    "vec3 toGammaSpace(vec3 color)\n"
    "{\n"
    "  return pow(color, vec3(GammaEncodePowerApprox));\n"
    "}\n"
    "\n"
    "float square(float value)\n"
    "{\n"
    "  return value * value;\n"
    "}\n"
    "\n"
    "float getLuminance(vec3 color)\n"
    "{\n"
    "  return clamp(dot(color, LuminanceEncodeApprox), 0., 1.);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_HELPER_FUNCTIONS_FX_H
