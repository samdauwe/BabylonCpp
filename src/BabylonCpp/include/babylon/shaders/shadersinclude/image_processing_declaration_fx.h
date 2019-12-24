#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_DECLARATION_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_DECLARATION_FX_H

namespace BABYLON {

extern const char* imageProcessingDeclaration;

const char* imageProcessingDeclaration
  = R"ShaderCode(

#ifdef EXPOSURE
    uniform float exposureLinear;
#endif

#ifdef CONTRAST
    uniform float contrast;
#endif

#ifdef VIGNETTE
    uniform vec2 vInverseScreenSize;
    uniform vec4 vignetteSettings1;
    uniform vec4 vignetteSettings2;
#endif

#ifdef COLORCURVES
    uniform vec4 vCameraColorCurveNegative;
    uniform vec4 vCameraColorCurveNeutral;
    uniform vec4 vCameraColorCurvePositive;
#endif

#ifdef COLORGRADING
    #ifdef COLORGRADING3D
        uniform highp sampler3D txColorTransform;
    #else
        uniform sampler2D txColorTransform;
    #endif
    uniform vec4 colorTransformSettings;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_DECLARATION_FX_H
