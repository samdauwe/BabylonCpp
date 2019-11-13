#include <babylon/materials/textures/refraction_texture.h>

#include <nlohmann/json.hpp>

#include <babylon/engines/scene.h>

namespace BABYLON {

RefractionTexture::RefractionTexture(const std::string& iName, ISize size,
                                     Scene* scene, bool generateMipMaps)
    : RenderTargetTexture(iName, size, scene, generateMipMaps, true)
    , refractionPlane{Plane(0.f, 1.f, 0.f, 1.f)}
    , depth{2.f}
{
  onBeforeRenderObservable.add(
    [this](int*, EventState&) { getScene()->clipPlane = refractionPlane; });

  onAfterRenderObservable.add(
    [this](int*, EventState&) { getScene()->clipPlane = std::nullopt; });
}

RefractionTexture::~RefractionTexture() = default;

std::unique_ptr<RefractionTexture> RefractionTexture::clone()
{
  auto scene = getScene();

  if (!scene) {
    return nullptr;
  }

  auto textureSize = getSize();
  auto newTexture  = std::make_unique<RefractionTexture>(
    name, Size(textureSize.width, textureSize.height), scene, _generateMipMaps);

  // Base texture
  newTexture->hasAlpha = hasAlpha();
  newTexture->level    = level;

  // Mirror Texture
  newTexture->refractionPlane = refractionPlane;
  if (!renderList().empty()) {
    newTexture->renderList = renderList();
  }
  newTexture->depth = depth;

  return newTexture;
}

json RefractionTexture::serialize() const
{
  return nullptr;
}

} // end of namespace BABYLON
