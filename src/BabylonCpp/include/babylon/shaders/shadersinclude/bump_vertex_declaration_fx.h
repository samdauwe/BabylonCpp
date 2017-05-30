#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* bumpVertexDeclaration;

const char* bumpVertexDeclaration
  = "#if defined(BUMP) || defined(PARALLAX)\n"
    "  #if defined(TANGENT) && defined(NORMAL) \n"
    "  varying mat3 vTBN;\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_DECLARATION_FX_H
