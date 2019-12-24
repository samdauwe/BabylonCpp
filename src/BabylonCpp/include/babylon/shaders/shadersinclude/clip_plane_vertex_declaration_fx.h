#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* clipPlaneVertexDeclaration;

const char* clipPlaneVertexDeclaration
  = R"ShaderCode(

#ifdef CLIPPLANE
    uniform vec4 vClipPlane;
    varying float fClipDistance;
#endif

#ifdef CLIPPLANE2
    uniform vec4 vClipPlane2;
    varying float fClipDistance2;
#endif

#ifdef CLIPPLANE3
    uniform vec4 vClipPlane3;
    varying float fClipDistance3;
#endif

#ifdef CLIPPLANE4
    uniform vec4 vClipPlane4;
    varying float fClipDistance4;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_CLIP_PLANE_VERTEX_DECLARATION_FX_H
