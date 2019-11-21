#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_DECLARATION_FX_H

namespace BABYLON {

extern const char* instancesDeclaration;

const char* instancesDeclaration
  = R"ShaderCode(

#ifdef INSTANCES
    attribute vec4 world0;
    attribute vec4 world1;
    attribute vec4 world2;
    attribute vec4 world3;
#else
    uniform mat4 world;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_DECLARATION_FX_H
