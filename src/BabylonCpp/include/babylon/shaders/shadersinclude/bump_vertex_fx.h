#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H

namespace BABYLON {

extern const char* bumpVertex;

const char* bumpVertex
  = "#if defined(BUMP) || defined(PARALLAX)\n"
    "  #if defined(TANGENT) && defined(NORMAL)\n"
    "  vec3 normalW = normalize(vec3(finalWorld * vec4(normalUpdated, 0.0)));\n"
    "  vec3 tangentW = normalize(vec3(finalWorld * vec4(tangentUpdated.xyz, 0.0)));\n"
    "  vec3 bitangentW = cross(normalW, tangentW) * tangentUpdated.w;\n"
    "  vTBN = mat3(tangentW, bitangentW, normalW);\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_FX_H
