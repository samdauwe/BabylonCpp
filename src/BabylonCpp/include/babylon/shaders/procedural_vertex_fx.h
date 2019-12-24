#ifndef BABYLON_SHADERS_PROCEDURAL_VERTEX_FX_H
#define BABYLON_SHADERS_PROCEDURAL_VERTEX_FX_H

namespace BABYLON {

extern const char* proceduralVertexShader;

const char* proceduralVertexShader
  = R"ShaderCode(

// Attributes
attribute vec2 position;

// Output
varying vec2 vPosition;
varying vec2 vUV;

const vec2 madd = vec2(0.5, 0.5);

void main(void) {
    vPosition = position;
    vUV = position * madd + madd;
    gl_Position = vec4(position, 0.0, 1.0);
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_PROCEDURAL_VERTEX_FX_H
