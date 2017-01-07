#ifndef BABYLON_MATERIALS_LIBRARY_SKY_SKY_VERTEX_FX_H
#define BABYLON_MATERIALS_LIBRARY_SKY_SKY_VERTEX_FX_H

namespace BABYLON {

extern const char* skyVertexShader;

const char* skyVertexShader
  = "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "\n"
    "// Attributes\n"
    "attribute vec3 position;\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "attribute vec4 color;\n"
    "#endif\n"
    "\n"
    "// Uniforms\n"
    "uniform mat4 world;\n"
    "uniform mat4 view;\n"
    "uniform mat4 viewProjection;\n"
    "\n"
    "#ifdef POINTSIZE\n"
    "uniform float pointSize;\n"
    "#endif\n"
    "\n"
    "// Output\n"
    "varying vec3 vPositionW;\n"
    "\n"
    "#ifdef VERTEXCOLOR\n"
    "varying vec4 vColor;\n"
    "#endif\n"
    "\n"
    "#include<clipPlaneVertexDeclaration>\n"
    "#include<fogVertexDeclaration>\n"
    "\n"
    "void main(void) {\n"
    "  gl_Position = viewProjection * world * vec4(position, 1.0);\n"
    "  \n"
    "  vec4 worldPos = world * vec4(position, 1.0);\n"
    "  vPositionW = vec3(worldPos);\n"
    "\n"
    "  // Clip plane\n"
    "#include<clipPlaneVertex>\n"
    "\n"
    "  // Fog\n"
    "#include<fogVertex>\n"
    "\n"
    "  // Vertex color\n"
    "#ifdef VERTEXCOLOR\n"
    "  vColor = color;\n"
    "#endif\n"
    "\n"
    "  // Point size\n"
    "#ifdef POINTSIZE\n"
    "  gl_PointSize = pointSize;\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SKY_SKY_VERTEX_FX_H
