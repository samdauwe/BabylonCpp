#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* fogVertexDeclaration;

const char* fogVertexDeclaration
  = R"ShaderCode(

#ifdef FOG
    varying vec3 vFogDistance;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_DECLARATION_FX_H
