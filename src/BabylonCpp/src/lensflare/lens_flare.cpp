#include <babylon/lensflare/lens_flare.h>

#include <babylon/engine/engine.h>
#include <babylon/lensflare/lens_flare_system.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

LensFlare::LensFlare(float iSize, const Vector3& iPosition,
                     const Color3& iColor, const std::string& imgUrl,
                     LensFlareSystem* system)
    : size{iSize}
    , position{iPosition}
    , color{iColor}
    , texture{!imgUrl.empty() ? Texture::New(imgUrl, system->getScene(), true) :
                                nullptr}
    , alphaMode{EngineConstants::ALPHA_ONEONE}
    , _system{system}
{
}

LensFlare::~LensFlare()
{
}

void LensFlare::addToLensFlareSystem(const LensFlarePtr& lensFlare)
{
  _system->lensFlares.emplace_back(lensFlare);
}

LensFlarePtr LensFlare::AddFlare(float size, const Vector3& position,
                                 const Color3& color, const std::string& imgUrl,
                                 LensFlareSystem* system)
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
  _system->lensFlares.erase(
    std::remove_if(_system->lensFlares.begin(), _system->lensFlares.end(),
                   [this](const LensFlarePtr& lensFlare) {
                     return lensFlare.get() == this;
                   }),
    _system->lensFlares.end());
}

} // end of namespace BABYLON
