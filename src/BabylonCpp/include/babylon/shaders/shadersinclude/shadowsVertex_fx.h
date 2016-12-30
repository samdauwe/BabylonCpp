#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWSVERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWSVERTEX_FX_H

namespace BABYLON {

extern const char* shadowsVertex;

const char* shadowsVertex
  = "#ifdef SHADOWS\n"
    "  #if defined(SPOTLIGHT{X}) || defined(DIRLIGHT{X})\n"
    "  vPositionFromLight{X} = lightMatrix{X} * worldPos;\n"
    "  #endif\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_SHADOWSVERTEX_FX_H
