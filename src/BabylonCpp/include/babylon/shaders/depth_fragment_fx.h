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
    "varying float vDepthMetric;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "#ifdef ALPHATEST\n"
    "  if (texture2D(diffuseSampler, vUV).a < 0.4)\n"
    "  discard;\n"
    "#endif\n"
    "\n"
    "  gl_FragColor = vec4(vDepthMetric, vDepthMetric * vDepthMetric, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DEPTH_FRAGMENT_FX_H
