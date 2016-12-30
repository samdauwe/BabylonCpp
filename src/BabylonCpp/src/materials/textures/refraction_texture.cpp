#include <babylon/materials/textures/refraction_texture.h>

#include <babylon/core/json.h>
#include <babylon/engine/scene.h>

namespace BABYLON {

RefractionTexture::RefractionTexture(const std::string& iName, ISize size,
                                     Scene* scene, bool generateMipMaps)
    : RenderTargetTexture(iName, size, scene, generateMipMaps, true)
    , refractionPlane{Plane(0.f, 1.f, 0.f, 1.f)}
    , depth{2.f}
{
  onBeforeRenderObservable.add([&]() { scene->setClipPlane(refractionPlane); });

  onAfterRenderObservable.add([&]() { scene->resetClipPlane(); });
}

RefractionTexture::~RefractionTexture()
{
}

std::unique_ptr<RefractionTexture> RefractionTexture::clone() const
{
  /*auto textureSize = getSize();
  auto newTexture  = std_util::make_unique<RefractionTexture>(
    name, textureSize.width, getScene(), _generateMipMaps);

  // Base texture
  newTexture->hasAlpha = hasAlpha;
  newTexture->level    = level;

  // Mirror Texture
  newTexture->refractionPlane = refractionPlane;
  newTexture->renderList      = renderList;
  newTexture->depth           = depth;

  return newTexture;*/
  return nullptr;
}

Json::object RefractionTexture::serialize() const
{
  return Json::object();
}

} // end of namespace BABYLON
