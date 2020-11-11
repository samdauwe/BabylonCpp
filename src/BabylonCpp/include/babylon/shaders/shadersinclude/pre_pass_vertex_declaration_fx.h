#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PRE_PASS_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PRE_PASS_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* prePassVertexDeclaration;

const char* prePassVertexDeclaration
  = R"ShaderCode(

#ifdef PREPASS
#ifdef PREPASS_DEPTHNORMAL
    varying vec3 vViewPos;
#endif
#ifdef PREPASS_VELOCITY
    uniform mat4 previousWorld;
    uniform mat4 previousViewProjection;
    varying vec4 vCurrentPosition;
    varying vec4 vPreviousPosition;
#endif
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PRE_PASS_VERTEX_DECLARATION_FX_H
