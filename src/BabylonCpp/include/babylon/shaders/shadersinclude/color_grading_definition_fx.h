#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_GRADING_DEFINITION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_GRADING_DEFINITION_FX_H

namespace BABYLON {

extern const char* colorGradingDefinition;

const char* colorGradingDefinition
  = "uniform sampler2D cameraColorGrading2DSampler;\n"
    "uniform vec4 vCameraColorGradingInfos;\n"
    "uniform vec4 vCameraColorGradingScaleOffset;\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_COLOR_GRADING_DEFINITION_FX_H
