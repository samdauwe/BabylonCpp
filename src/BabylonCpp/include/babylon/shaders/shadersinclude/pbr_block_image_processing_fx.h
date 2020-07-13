#ifndef BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_IMAGE_PROCESSING_FX_H
#define BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_IMAGE_PROCESSING_FX_H

namespace BABYLON {

extern const char* pbrBlockImageProcessing;

const char* pbrBlockImageProcessing
  = R"ShaderCode(

#ifdef IMAGEPROCESSINGPOSTPROCESS
    // Sanitize output incase invalid normals or tangents have caused div by 0 or undefined behavior
    // this also limits the brightness which helpfully reduces over-sparkling in bloom (native handles this in the bloom blur shader)
    finalColor.rgb = clamp(finalColor.rgb, 0., 30.0);
#else
    // Alway run to ensure we are going back to gamma space.
    finalColor = applyImageProcessing(finalColor);
#endif

    finalColor.a *= visibility;

#ifdef PREMULTIPLYALPHA
    // Convert to associative (premultiplied) format if needed.
    finalColor.rgb *= finalColor.a;
#endif

)ShaderCode";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_SHADERS_INCLUDE_PBR_BLOCK_IMAGE_PROCESSING_FX_H
