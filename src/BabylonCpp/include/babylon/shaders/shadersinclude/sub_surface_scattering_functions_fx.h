#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SUB_SURFACE_SCATTERING_FUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SUB_SURFACE_SCATTERING_FUNCTIONS_FX_H

namespace BABYLON {

extern const char* subSurfaceScatteringFunctions;

const char* subSurfaceScatteringFunctions
  = R"ShaderCode(

bool testLightingForSSS(float diffusionProfile)
{
    return diffusionProfile < 1.;
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SUB_SURFACE_SCATTERING_FUNCTIONS_FX_H
