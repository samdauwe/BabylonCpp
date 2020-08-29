#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_DIFFUSION_PROFILE_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_DIFFUSION_PROFILE_FX_H

namespace BABYLON {

extern const char* diffusionProfile;

const char* diffusionProfile
  = R"ShaderCode(

uniform vec3 diffusionS[5];
uniform float diffusionD[5];
uniform float filterRadii[5];

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_DIFFUSION_PROFILE_FX_H
