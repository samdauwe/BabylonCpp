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
    "uniform mat4 worldViewProjection;\n"
    "uniform mat4 world;\n"
    "uniform mat4 view;\n"
    "\n"
    "// Varying\n"
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
    "  gl_Position = worldViewProjection * vec4(position, 1.0);\n"
    "\n"
    "  vPosition = position;\n"
    "  vNormal = normal;\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_GRID_GRID_VERTEX_FX_H
