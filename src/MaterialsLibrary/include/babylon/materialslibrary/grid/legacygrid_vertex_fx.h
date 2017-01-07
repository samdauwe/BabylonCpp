#ifndef BABYLON_MATERIALS_LIBRARY_GRID_LEGACYGRID_VERTEX_FX_H
#define BABYLON_MATERIALS_LIBRARY_GRID_LEGACYGRID_VERTEX_FX_H

namespace BABYLON {

extern const char* legacygridVertexShader;

const char* legacygridVertexShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "// Attributes\n"
    "attribute vec3 position;\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 worldViewProjection;\n"
    "\n"
    "void main(void) {\n"
    "  gl_Position = worldViewProjection * vec4(position, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_GRID_LEGACYGRID_VERTEX_FX_H
