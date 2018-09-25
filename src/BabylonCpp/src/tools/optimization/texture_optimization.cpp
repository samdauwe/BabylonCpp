#include <babylon/tools/optimization/texture_optimization.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

TextureOptimization::TextureOptimization(int iPriority, int iMaximumSize)
    : SceneOptimization{iPriority}, maximumSize{iMaximumSize}
{
}

TextureOptimization::~TextureOptimization()
{
}

bool TextureOptimization::apply(Scene* scene)
{
  bool allDone = true;
  for (auto& texture : scene->textures) {
    if (!texture->canRescale()) {
      continue;
    }

    auto currentSize  = texture->getSize();
    auto maxDimension = std::max(currentSize.width, currentSize.height);

    if (maxDimension > maximumSize) {
      texture->scale(0.5f);
      allDone = false;
    }
  }

  return allDone;
}

} // end of namespace BABYLON
