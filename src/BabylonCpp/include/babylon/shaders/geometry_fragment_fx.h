#ifndef BABYLON_SHADERS_GEOMETRY_FRAGMENT_FX_H
#define BABYLON_SHADERS_GEOMETRY_FRAGMENT_FX_H

namespace BABYLON {

extern const char* geometryPixelShader;

const char* geometryPixelShader
  = "#version 300 es\n"
    "\n"
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "precision highp int;\n"
    "\n"
    "in vec3 vNormalV;\n"
    "in vec4 vViewPos;\n"
    "\n"
    "#ifdef POSITION\n"
    "in vec3 vPosition;\n"
    "#endif\n"
    "\n"
    "#ifdef ALPHATEST\n"
    "in vec2 vUV;\n"
    "uniform sampler2D diffuseSampler;\n"
    "#endif\n"
    "\n"
    "layout(location = 0) out vec4 color0;\n"
    "layout(location = 1) out vec4 color1;\n"
    "\n"
    "#ifdef POSITION\n"
    "layout(location = 2) out vec4 color2;\n"
    "#endif\n"
    "\n"
    "void main() {\n"
    "#ifdef ALPHATEST\n"
    "  if (texture(diffuseSampler, vUV).a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "\n"
    "  color0 = vec4(vViewPos.z / vViewPos.w, 0.0, 0.0, 1.0);\n"
    "  color1 = vec4(normalize(vNormalV), 1.0);\n"
    "  //color2 = vec4(vPositionV, 1.0);\n"
    "\n"
    "  #ifdef POSITION\n"
    "  color2 = vec4(vPosition, 1.0);\n"
    "  #endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GEOMETRY_FRAGMENT_FX_H
