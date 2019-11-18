#include <babylon/lensflares/lens_flare.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/lensflares/lens_flare_system.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

LensFlare::LensFlare(float iSize, float iPosition, const Color3& iColor,
                     const std::string& imgUrl,
                     const LensFlareSystemPtr& system)
    : color{iColor}
    , texture{!imgUrl.empty() ? Texture::New(imgUrl, system->getScene(), true) :
                                nullptr}
    , alphaMode{Constants::ALPHA_ONEONE}
    , size{iSize}
    , position{iPosition}
    , _system{system}
{
}

LensFlare::~LensFlare() = default;

void LensFlare::addToLensFlareSystem(const LensFlarePtr& lensFlare)
{
  _system->lensFlares.emplace_back(lensFlare);
}

LensFlarePtr LensFlare::AddFlare(float size, float position,
                                 const Color3& color, const std::string& imgUrl,
                                 const LensFlareSystemPtr& system)
{
  return LensFlare::New(size, position, color, imgUrl, system);
}

void LensFlare::dispose()
{
  if (texture) {
    texture->dispose();
    texture = nullptr;
  }

  // Remove from scene
  stl_util::remove_vector_elements_equal_sharedptr(_system->lensFlares, this);
}

} // end of namespace BABYLON
