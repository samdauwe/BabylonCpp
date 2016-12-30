#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCESDECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCESDECLARATION_FX_H

namespace BABYLON {

extern const char* instancesDeclaration;

const char* instancesDeclaration
  = "#ifdef INSTANCES\n"
    "  attribute vec4 world0;\n"
    "  attribute vec4 world1;\n"
    "  attribute vec4 world2;\n"
    "  attribute vec4 world3;\n"
    "#else\n"
    "  uniform mat4 world;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_INSTANCESDECLARATION_FX_H
