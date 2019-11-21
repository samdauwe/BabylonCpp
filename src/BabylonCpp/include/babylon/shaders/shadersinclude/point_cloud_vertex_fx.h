#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_FX_H

namespace BABYLON {

extern const char* pointCloudVertex;

const char* pointCloudVertex
  = R"ShaderCode(

#ifdef POINTSIZE
    gl_PointSize = pointSize;
#endif

)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_FX_H
