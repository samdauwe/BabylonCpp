#ifndef BABYLON_SHADERS_DEPTH_FRAGMENT_FX_H
#define BABYLON_SHADERS_DEPTH_FRAGMENT_FX_H

namespace BABYLON {

extern const char* depthPixelShader;

const char* depthPixelShader
  = "#ifdef ALPHATEST\n"
    "varying vec2 vUV;\n"
    "uniform sampler2D diffuseSampler;\n"
    "#endif\n"
    "\n"
    "uniform float far;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "#ifdef ALPHATEST\n"
    "  if (texture2D(diffuseSampler, vUV).a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "\n"
    "  float depth = (gl_FragCoord.z / gl_FragCoord.w) / far;\n"
    "  gl_FragColor = vec4(depth, depth * depth, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DEPTH_FRAGMENT_FX_H
