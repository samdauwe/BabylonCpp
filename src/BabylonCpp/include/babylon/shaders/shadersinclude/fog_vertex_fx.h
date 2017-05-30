#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_FX_H

namespace BABYLON {

extern const char* fogVertex;

const char* fogVertex
  = "#ifdef FOG\n"
    "vFogDistance = (view * worldPos).xyz;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_FOG_VERTEX_FX_H
