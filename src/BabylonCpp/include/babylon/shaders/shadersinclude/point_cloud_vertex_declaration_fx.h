#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* pointCloudVertexDeclaration;

const char* pointCloudVertexDeclaration
  = R"ShaderCode(

#ifdef POINTSIZE
    uniform float pointSize;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_DECLARATION_FX_H
