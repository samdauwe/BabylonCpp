#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_NORMAL_FINAL_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_NORMAL_FINAL_FX_H

namespace BABYLON {

extern const char* pbrBlockNormalFinal;

const char* pbrBlockNormalFinal
  = R"ShaderCode(

#if defined(FORCENORMALFORWARD) && defined(NORMAL)
    vec3 faceNormal = normalize(cross(dFdx(vPositionW), dFdy(vPositionW))) * vEyePosition.w;
    #if defined(TWOSIDEDLIGHTING)
        faceNormal = gl_FrontFacing ? faceNormal : -faceNormal;
    #endif

    normalW *= sign(dot(normalW, faceNormal));
#endif

#if defined(TWOSIDEDLIGHTING) && defined(NORMAL)
    normalW = gl_FrontFacing ? normalW : -normalW;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_NORMAL_FINAL_FX_H
