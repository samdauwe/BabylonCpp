#include <babylon/loading/glTF/2.0/extensions/msft_srgbFactors.h>

namespace BABYLON {
namespace GLTF2 {

MSFT_sRGBFactors::MSFT_sRGBFactors(const IGLTFLoaderPtr& loader)
    : _loader{loader}
{
  name    = MSFT_sRGBFactors::NAME;
  enabled = true;
}

void MSFT_sRGBFactors::dispose(bool /*doNotRecurse*/,
                               bool /*disposeMaterialAndTextures*/)
{
}

void MSFT_sRGBFactors::loadMaterialPropertiesAsync(
  const std::string& /*context*/, const IMaterial& /*material*/,
  const Material& /*babylonMaterial*/)
{
}

} // end of namespace GLTF2
} // end of namespace BABYLON
