#ifndef BABYLON_MATERIALS_LIBRARY_GRID_GRID_VERTEX_FX_H
#define BABYLON_MATERIALS_LIBRARY_GRID_GRID_VERTEX_FX_H

namespace BABYLON {

extern const char* gridVertexShader;

const char* gridVertexShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "// Attributes\n"
    "attribute vec3 position;\n"
    "attribute vec3 normal;\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 projection;\n"
    "uniform mat4 world;\n"
    "uniform mat4 view;\n"
    "uniform mat4 worldView;\n"
    "\n"
    "// Varying\n"
    "#ifdef TRANSPARENT\n"
    "  varying vec4 vCameraSpacePosition;\n"
    "#endif\n"
    "varying vec3 vPosition;\n"
    "varying vec3 vNormal;\n"
    "\n"
    "#include<fogVertexDeclaration>\n"
    "\n"
    "void main(void) {\n"
    "\n"
    "  #ifdef FOG\n"
    "  vec4 worldPos = world * vec4(position, 1.0);\n"
    "  #endif\n"
    "\n"
    "  #include<fogVertex>\n"
    "\n"
    "  vec4 cameraSpacePosition = worldView * vec4(position, 1.0);\n"
    "  gl_Position = projection * cameraSpacePosition;\n"
    "\n"
    "  #ifdef TRANSPARENT\n"
    "  vCameraSpacePosition = cameraSpacePosition;\n"
    "  #endif\n"
    "\n"
    "  vPosition = position;\n"
    "  vNormal = normal;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_GRID_GRID_VERTEX_FX_H
