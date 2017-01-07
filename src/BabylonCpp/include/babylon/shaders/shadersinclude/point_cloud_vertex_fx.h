#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_FX_H

namespace BABYLON {

extern const char* pointCloudVertex;

const char* pointCloudVertex
  = "#ifdef POINTSIZE\n"
    "  gl_PointSize = pointSize;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_FX_H
