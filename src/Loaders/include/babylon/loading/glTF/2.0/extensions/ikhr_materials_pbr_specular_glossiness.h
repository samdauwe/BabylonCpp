#ifndef BABYLON_LOADING_GLTF_2_0_EXTENSIONS_IKHR_MATERIALS_PBR_SPECULAR_GLOSSINESSS_H
#define BABYLON_LOADING_GLTF_2_0_EXTENSIONS_IKHR_MATERIALS_PBR_SPECULAR_GLOSSINESSS_H

#include <babylon/babylon_global.h>
#include <babylon/loading/glTF/2.0/gltf_loader_interfaces.h>

#include <babylon/core/nullable.h>

namespace BABYLON {
namespace GLTF2 {

/**
 * @brief
 */
struct BABYLON_SHARED_EXPORT IKHRMaterialsPbrSpecularGlossiness {
  Float32Array diffuseFactor;
  Nullable<IGLTFTextureInfo> diffuseTexture;
  Float32Array specularFactor;
  Nullable<float> glossinessFactor;
  Nullable<IGLTFTextureInfo> specularGlossinessTexture;
}; // end of struct IKHRMaterialsPbrSpecularGlossiness

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of
// BABYLON_LOADING_GLTF_2_0_EXTENSIONS_IKHR_MATERIALS_PBR_SPECULAR_GLOSSINESSS_H
