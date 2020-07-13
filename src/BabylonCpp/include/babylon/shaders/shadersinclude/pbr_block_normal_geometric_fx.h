#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_NORMAL_GEOMETRIC_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_NORMAL_GEOMETRIC_FX_H

namespace BABYLON {

extern const char* pbrBlockNormalGeometric;

const char* pbrBlockNormalGeometric
  = R"ShaderCode(

vec3 viewDirectionW = normalize(vEyePosition.xyz - vPositionW);

#ifdef NORMAL
    vec3 normalW = normalize(vNormalW);
#else
    vec3 normalW = normalize(cross(dFdx(vPositionW), dFdy(vPositionW))) * vEyePosition.w;
#endif

vec3 geometricNormalW = normalW;

#if defined(TWOSIDEDLIGHTING) && defined(NORMAL)
    geometricNormalW = gl_FrontFacing ? geometricNormalW : -geometricNormalW;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_NORMAL_GEOMETRIC_FX_H
