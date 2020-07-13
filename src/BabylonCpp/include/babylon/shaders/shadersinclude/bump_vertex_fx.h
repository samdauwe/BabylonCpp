#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H

namespace BABYLON {

extern const char* bumpVertex;

const char* bumpVertex
  = R"ShaderCode(

#if defined(BUMP) || defined(PARALLAX) || defined(CLEARCOAT_BUMP) || defined(ANISOTROPIC)
    #if defined(TANGENT) && defined(NORMAL)
        vec3 tbnNormal = normalize(normalUpdated);
        vec3 tbnTangent = normalize(tangentUpdated.xyz);
        vec3 tbnBitangent = cross(tbnNormal, tbnTangent) * tangentUpdated.w;
        vTBN = mat3(finalWorld) * mat3(tbnTangent, tbnBitangent, tbnNormal);
    #endif
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H
