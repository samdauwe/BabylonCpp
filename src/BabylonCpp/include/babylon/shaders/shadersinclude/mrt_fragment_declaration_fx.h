#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_MRT_FRAGMENT_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_MRT_FRAGMENT_DECLARATION_FX_H

namespace BABYLON {

extern const char* mrtFragmentDeclaration;

const char* mrtFragmentDeclaration
  = "#if __VERSION__ >= 200\n"
    "layout(location = 0) out vec4 glFragData[{X}];\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_MRT_FRAGMENT_DECLARATION_FX_H
