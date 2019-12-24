#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_FX_H

namespace BABYLON {

extern const char* fogVertex;

const char* fogVertex
  = R"ShaderCode(

#ifdef FOG
vFogDistance = (view * worldPos).xyz;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_FX_H
