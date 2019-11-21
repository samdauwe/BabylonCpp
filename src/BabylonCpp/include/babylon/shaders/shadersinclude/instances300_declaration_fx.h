#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES300_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES300_DECLARATION_FX_H

namespace BABYLON {

extern const char* instances300Declaration;

const char* instances300Declaration
  = R"ShaderCode(

#ifdef INSTANCES
    in vec4 world0;
    in vec4 world1;
    in vec4 world2;
    in vec4 world3;
#else
    uniform mat4 world;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES300_DECLARATION_FX_H
