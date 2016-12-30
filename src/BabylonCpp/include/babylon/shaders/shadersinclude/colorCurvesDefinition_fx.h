#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_COLORCURVESDEFINITION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_COLORCURVESDEFINITION_FX_H

namespace BABYLON {

extern const char* colorCurvesDefinition;

const char* colorCurvesDefinition
  = "uniform vec4 vCameraColorCurveNeutral;\n"
    "uniform vec4 vCameraColorCurvePositive;\n"
    "uniform vec4 vCameraColorCurveNegative;\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_COLORCURVESDEFINITION_FX_H
