#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PRE_PASS_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PRE_PASS_DECLARATION_FX_H

namespace BABYLON {

extern const char* prePassDeclaration;

const char* prePassDeclaration
  = R"ShaderCode(

#ifdef PREPASS
#extension GL_EXT_draw_buffers : require
layout(location = 0) out vec4 glFragData[{X}];
vec4 gl_FragColor;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PRE_PASS_DECLARATION_FX_H
