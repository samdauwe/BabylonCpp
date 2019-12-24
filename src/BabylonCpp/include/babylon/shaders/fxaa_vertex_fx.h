#ifndef BABYLON_SHADERS_FXAA_VERTEX_FX_H
#define BABYLON_SHADERS_FXAA_VERTEX_FX_H

namespace BABYLON {

extern const char* fxaaVertexShader;

const char* fxaaVertexShader
  = R"ShaderCode(

// Attributes
attribute vec2 position;
uniform vec2 texelSize;

// Output
varying vec2 vUV;
varying vec2 sampleCoordS;
varying vec2 sampleCoordE;
varying vec2 sampleCoordN;
varying vec2 sampleCoordW;
varying vec2 sampleCoordNW;
varying vec2 sampleCoordSE;
varying vec2 sampleCoordNE;
varying vec2 sampleCoordSW;

const vec2 madd = vec2(0.5, 0.5);

void main(void) {
    vUV = (position * madd + madd);

    sampleCoordS = vUV + vec2( 0.0, 1.0) * texelSize;
    sampleCoordE = vUV + vec2( 1.0, 0.0) * texelSize;
    sampleCoordN = vUV + vec2( 0.0,-1.0) * texelSize;
    sampleCoordW = vUV + vec2(-1.0, 0.0) * texelSize;

    sampleCoordNW = vUV + vec2(-1.0,-1.0) * texelSize;
    sampleCoordSE = vUV + vec2( 1.0, 1.0) * texelSize;
    sampleCoordNE = vUV + vec2( 1.0,-1.0) * texelSize;
    sampleCoordSW = vUV + vec2(-1.0, 1.0) * texelSize;

    gl_Position = vec4(position, 0.0, 1.0);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_FXAA_VERTEX_FX_H
