#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_VERTEX_FX_H

namespace BABYLON {

extern const char* instancesVertex;

const char* instancesVertex
  = R"ShaderCode(

#ifdef INSTANCES
    mat4 finalWorld = mat4(world0, world1, world2, world3);
#else
    mat4 finalWorld = world;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_VERTEX_FX_H
