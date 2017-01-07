#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_VERTEX_FX_H

namespace BABYLON {

extern const char* instancesVertex;

const char* instancesVertex
  = "#ifdef INSTANCES\n"
    "  mat4 finalWorld = mat4(world0, world1, world2, world3);\n"
    "#else\n"
    "  mat4 finalWorld = world;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCES_VERTEX_FX_H
