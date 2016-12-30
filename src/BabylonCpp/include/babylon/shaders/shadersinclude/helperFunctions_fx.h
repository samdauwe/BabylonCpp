#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_HELPERFUNCTIONS_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_HELPERFUNCTIONS_FX_H

namespace BABYLON {

extern const char* helperFunctions;

const char* helperFunctions
  = "mat3 transposeMat3(mat3 inMatrix) {\n"
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
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_HELPERFUNCTIONS_FX_H
