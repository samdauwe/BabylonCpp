#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_DECLARATION_FX_H

namespace BABYLON {

extern const char* imageProcessingDeclaration;

const char* imageProcessingDeclaration
  = "#ifdef EXPOSURE\n"
    "  uniform float exposureLinear;\n"
    "#endif\n"
    "\n"
    "#ifdef CONTRAST\n"
    "  uniform float contrast;\n"
    "#endif\n"
    "\n"
    "#ifdef VIGNETTE\n"
    "  uniform vec2 vInverseScreenSize;\n"
    "  uniform vec4 vignetteSettings1;\n"
    "  uniform vec4 vignetteSettings2;\n"
    "#endif\n"
    "\n"
    "#ifdef COLORCURVES\n"
    "  uniform vec4 vCameraColorCurveNegative;\n"
    "  uniform vec4 vCameraColorCurveNeutral;\n"
    "  uniform vec4 vCameraColorCurvePositive;\n"
    "#endif\n"
    "\n"
    "#ifdef COLORGRADING\n"
    "  uniform sampler2D txColorTransform;\n"
    "  uniform vec4 colorTransformSettings;\n"
    "#endif\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_DECLARATION_FX_H
