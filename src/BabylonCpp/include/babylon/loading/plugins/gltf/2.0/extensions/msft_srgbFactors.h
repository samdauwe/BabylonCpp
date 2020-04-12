#ifndef BABYLON_LOADING_PLUGINS_GLTF_2_0_EXTENSIONS_MSFT_SRGBFACTORS_H
#define BABYLON_LOADING_PLUGINS_GLTF_2_0_EXTENSIONS_MSFT_SRGBFACTORS_H

#include <babylon/babylon_api.h>
#include <babylon/loading/plugins/gltf/2.0/gltf_loader_extension.h>

namespace BABYLON {
namespace GLTF2 {

using IGLTFLoaderPtr = std::shared_ptr<IGLTFLoader>;

class BABYLON_SHARED_EXPORT MSFT_sRGBFactors : public IGLTFLoaderExtension {

public:
  static constexpr const char* NAME = "MSFT_sRGBFactors";

  MSFT_sRGBFactors(const IGLTFLoaderPtr& loader);
  ~MSFT_sRGBFactors() override = default;

  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

  void loadMaterialPropertiesAsync(const std::string& context, const IMaterial& material,
                                   const Material& babylonMaterial) override;

public:
  IGLTFLoaderPtr _loader;

}; // end of class MSFT_sRGBFactors

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_PLUGINS_GLTF_2_0_EXTENSIONS_MSFT_SRGBFACTORS_H
