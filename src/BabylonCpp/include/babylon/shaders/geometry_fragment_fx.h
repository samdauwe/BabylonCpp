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
    "#ifdef VELOCITY\n"
    "varying vec4 vCurrentPosition;\n"
    "varying vec4 vPreviousPosition;\n"
    "#endif\n"
    "\n"
    "#ifdef ALPHATEST\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D diffuseSampler;\n"
    "#endif\n"
    "\n"
    "#include<mrtFragmentDeclaration>[RENDER_TARGET_COUNT]\n"
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
    "  gl_FragData[POSITION_INDEX] = vec4(vPosition, 1.0);\n"
    "  #endif\n"
    "\n"
    "  #ifdef VELOCITY\n"
    "  vec2 a = (vCurrentPosition.xy / vCurrentPosition.w) * 0.5 + 0.5;\n"
    "  vec2 b = (vPreviousPosition.xy / vPreviousPosition.w) * 0.5 + 0.5;\n"
    "\n"
    "  vec2 velocity = abs(a - b);\n"
    "  velocity = vec2(pow(velocity.x, 1.0 / 3.0), pow(velocity.y, 1.0 / 3.0)) * sign(a - b) * 0.5 + 0.5;\n"
    "\n"
    "  gl_FragData[VELOCITY_INDEX] = vec4(velocity, 0.0, 1.0);\n"
    "  #endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GEOMETRY_FRAGMENT_FX_H
