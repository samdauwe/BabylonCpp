#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* backgroundVertexDeclaration;

const char* backgroundVertexDeclaration
  = R"ShaderCode(

uniform mat4 view;
uniform mat4 viewProjection;
uniform float shadowLevel;

#ifdef DIFFUSE
uniform mat4 diffuseMatrix;
uniform vec2 vDiffuseInfos;
#endif

#ifdef REFLECTION
    uniform vec2 vReflectionInfos;
    uniform mat4 reflectionMatrix;
    uniform vec3 vReflectionMicrosurfaceInfos;
    uniform float fFovMultiplier;
#endif

#ifdef POINTSIZE
uniform float pointSize;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_BACKGROUND_VERTEX_DECLARATION_FX_H
