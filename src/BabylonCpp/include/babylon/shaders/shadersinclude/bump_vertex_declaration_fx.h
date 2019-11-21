#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* bumpVertexDeclaration;

const char* bumpVertexDeclaration
  = R"ShaderCode(

#if defined(BUMP) || defined(PARALLAX) || defined(CLEARCOAT_BUMP)
    #if defined(TANGENT) && defined(NORMAL)
        varying mat3 vTBN;
    #endif
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BUMP_VERTEX_DECLARATION_FX_H
