#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_COMPATIBILITY_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_COMPATIBILITY_FX_H

namespace BABYLON {

extern const char* imageProcessingCompatibility;

const char* imageProcessingCompatibility
  = R"ShaderCode(

#ifdef IMAGEPROCESSINGPOSTPROCESS
    gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(2.2));
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_IMAGE_PROCESSING_COMPATIBILITY_FX_H
