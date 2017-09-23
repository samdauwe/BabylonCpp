#ifndef BABYLON_SHADERS_GEOMETRY_FRAGMENT_FX_H
#define BABYLON_SHADERS_GEOMETRY_FRAGMENT_FX_H

namespace BABYLON {

extern const char* geometryPixelShader;

const char* geometryPixelShader
  = "#extension GL_EXT_draw_buffers : require\n"
    "\n"
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "precision highp int;\n"
    "\n"
    "varying vec3 vNormalV;\n"
    "varying vec4 vViewPos;\n"
    "\n"
    "#ifdef POSITION\n"
    "varying vec3 vPosition;\n"
    "#endif\n"
    "\n"
    "#ifdef ALPHATEST\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D diffuseSampler;\n"
    "#endif\n"
    "\n"
    "#ifdef POSITION\n"
    "#include<mrtFragmentDeclaration>[3]\n"
    "#else\n"
    "#include<mrtFragmentDeclaration>[2]\n"
    "#endif\n"
    "\n"
    "void main() {\n"
    "#ifdef ALPHATEST\n"
    "  if (texture2D(diffuseSampler, vUV).a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "\n"
    "  gl_FragData[0] = vec4(vViewPos.z / vViewPos.w, 0.0, 0.0, 1.0);\n"
    "  //color0 = vec4(vViewPos.z / vViewPos.w, 0.0, 0.0, 1.0);\n"
    "  gl_FragData[1] = vec4(normalize(vNormalV), 1.0);\n"
    "  //color2 = vec4(vPositionV, 1.0);\n"
    "\n"
    "  #ifdef POSITION\n"
    "  gl_FragData[2] = vec4(vPosition, 1.0);\n"
    "  #endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GEOMETRY_FRAGMENT_FX_H
