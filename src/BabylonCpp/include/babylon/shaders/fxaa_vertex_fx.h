#ifndef BABYLON_SHADERS_FXAA_VERTEX_FX_H
#define BABYLON_SHADERS_FXAA_VERTEX_FX_H

namespace BABYLON {

extern const char* fxaaVertexShader;

const char* fxaaVertexShader
  = "// Attributes\n"
    "attribute vec2 position;\n"
    "uniform vec2 texelSize;\n"
    "\n"
    "// Output\n"
    "varying vec2 vUV;\n"
    "varying vec2 sampleCoordS;\n"
    "varying vec2 sampleCoordE;\n"
    "varying vec2 sampleCoordN;\n"
    "varying vec2 sampleCoordW;\n"
    "varying vec2 sampleCoordNW;\n"
    "varying vec2 sampleCoordSE;\n"
    "varying vec2 sampleCoordNE;\n"
    "varying vec2 sampleCoordSW;\n"
    "\n"
    "const vec2 madd = vec2(0.5, 0.5);\n"
    "\n"
    "void main(void) {  \n"
    "  vUV = (position * madd + madd);\n"
    "\n"
    "  sampleCoordS = vUV + vec2( 0.0, 1.0) * texelSize;\n"
    "  sampleCoordE = vUV + vec2( 1.0, 0.0) * texelSize;\n"
    "  sampleCoordN = vUV + vec2( 0.0,-1.0) * texelSize;\n"
    "  sampleCoordW = vUV + vec2(-1.0, 0.0) * texelSize;\n"
    "\n"
    "  sampleCoordNW = vUV + vec2(-1.0,-1.0) * texelSize;\n"
    "  sampleCoordSE = vUV + vec2( 1.0, 1.0) * texelSize;\n"
    "  sampleCoordNE = vUV + vec2( 1.0,-1.0) * texelSize;\n"
    "  sampleCoordSW = vUV + vec2(-1.0, 1.0) * texelSize;\n"
    "\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_FXAA_VERTEX_FX_H
