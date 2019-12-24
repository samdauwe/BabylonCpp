#ifndef BABYLON_MATERIALS_LIBRARY_SKY_SKY_VERTEX_FX_H
#define BABYLON_MATERIALS_LIBRARY_SKY_SKY_VERTEX_FX_H

namespace BABYLON {

extern const char* skyVertexShader;

const char* skyVertexShader
  = R"ShaderCode(

#ifdef GL_ES
  precision highp float;
#endif

// Attributes
attribute vec3 position;

#ifdef VERTEXCOLOR
attribute vec4 color;
#endif

// Uniforms
uniform mat4 world;
uniform mat4 view;
uniform mat4 viewProjection;

#ifdef POINTSIZE
uniform float pointSize;
#endif

// Output
varying vec3 vPositionW;

#ifdef VERTEXCOLOR
varying vec4 vColor;
#endif

#include<clipPlaneVertexDeclaration>
#include<fogVertexDeclaration>

void main(void) {
    gl_Position = viewProjection * world * vec4(position, 1.0);

    vec4 worldPos = world * vec4(position, 1.0);
    vPositionW = vec3(worldPos);

    // Clip plane
#include<clipPlaneVertex>

    // Fog
#include<fogVertex>

    // Vertex color
#ifdef VERTEXCOLOR
    vColor = color;
#endif

    // Point size
#ifdef POINTSIZE
    gl_PointSize = pointSize;
#endif
}

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_SKY_SKY_VERTEX_FX_H
