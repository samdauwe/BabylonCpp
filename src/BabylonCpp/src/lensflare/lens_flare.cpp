#include <babylon/lensflare/lens_flare.h>

#include <babylon/engine/engine.h>
#include <babylon/lensflare/lens_flare_system.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

template <typename... Ts>
LensFlare* LensFlare::New(Ts&&... args)
{
  auto lensFlare = ::std::make_unique<LensFlare>(std::forward<Ts>(args)...);
  lensFlare->_system->lensFlares.emplace_back(lensFlare);
  return lensFlare.get();
}

LensFlare::LensFlare(float iSize, const Vector3& iPosition,
                     const Color3& iColor, const string_t& imgUrl,
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

void LensFlare::dispose(bool /*doNotRecurse*/)
{
  if (texture) {
    texture->dispose();
    texture = nullptr;
  }

  // Remove from scene
  _system->lensFlares.erase(
    ::std::remove_if(_system->lensFlares.begin(), _system->lensFlares.end(),
                     [this](const unique_ptr_t<LensFlare>& lensFlare) {
                       return lensFlare.get() == this;
                     }),
    _system->lensFlares.end());
}

} // end of namespace BABYLON
