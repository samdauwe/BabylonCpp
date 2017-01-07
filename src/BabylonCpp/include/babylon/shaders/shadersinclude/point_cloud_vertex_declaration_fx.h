#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_DECLARATION_FX_H

namespace BABYLON {

extern const char* pointCloudVertexDeclaration;

const char* pointCloudVertexDeclaration
  = "#ifdef POINTSIZE\n"
    "  uniform float pointSize;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_POINT_CLOUD_VERTEX_DECLARATION_FX_H
