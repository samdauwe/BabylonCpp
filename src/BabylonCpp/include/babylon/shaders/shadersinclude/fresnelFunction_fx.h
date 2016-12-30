#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FRESNELFUNCTION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FRESNELFUNCTION_FX_H

namespace BABYLON {

extern const char* fresnelFunction;

const char* fresnelFunction
  = "#ifdef FRESNEL\n"
    "  float computeFresnelTerm(vec3 viewDirection, vec3 worldNormal, float bias, float power)\n"
    "  {\n"
    "  float fresnelTerm = pow(bias + abs(dot(viewDirection, worldNormal)), power);\n"
    "  return clamp(fresnelTerm, 0., 1.);\n"
    "  }\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FRESNELFUNCTION_FX_H
