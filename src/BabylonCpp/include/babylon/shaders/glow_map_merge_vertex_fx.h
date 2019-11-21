#ifndef BABYLON_SHADERS_GLOW_MAP_MERGE_VERTEX_FX_H
#define BABYLON_SHADERS_GLOW_MAP_MERGE_VERTEX_FX_H

namespace BABYLON {

extern const char* glowMapMergeVertexShader;

const char* glowMapMergeVertexShader
  = R"ShaderCode(

// Attributes
attribute vec2 position;

// Output
varying vec2 vUV;

const vec2 madd = vec2(0.5, 0.5);

void main(void) {
    vUV = position * madd + madd;
    gl_Position = vec4(position, 0.0, 1.0);
}

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_GLOW_MAP_MERGE_VERTEX_FX_H
