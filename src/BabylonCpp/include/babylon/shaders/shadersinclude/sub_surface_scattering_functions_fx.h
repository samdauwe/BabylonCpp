#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SUB_SURFACE_SCATTERING_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SUB_SURFACE_SCATTERING_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* subSurfaceScatteringFunctions;

const char* subSurfaceScatteringFunctions
  = R"ShaderCode(

vec3 tagLightingForSSS(vec3 color) {
    color.b = max(color.b, HALF_MIN);

    return color;
}

bool testLightingForSSS(vec3 color)
{
    return color.b > 0.;
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SUB_SURFACE_SCATTERING_FUNCTIONS_FX_H
