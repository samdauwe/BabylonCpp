#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H

namespace BABYLON {

extern const char* bumpVertex;

const char* bumpVertex
  = "#if defined(BUMP) || defined(PARALLAX)\n"
    "  #if defined(TANGENT) && defined(NORMAL)\n"
    "  vec3 tbnNormal = normalize(normalUpdated);\n"
    "  vec3 tbnTangent = normalize(tangentUpdated.xyz);\n"
    "  vec3 tbnBitangent = cross(tbnNormal, tbnTangent) * tangentUpdated.w;\n"
    "  vTBN = mat3(finalWorld) * mat3(tbnTangent, tbnBitangent, tbnNormal);\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H
