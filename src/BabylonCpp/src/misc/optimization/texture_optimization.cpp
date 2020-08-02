#include <babylon/misc/optimization/texture_optimization.h>

#include <babylon/engines/scene.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

TextureOptimization::TextureOptimization(int iPriority, int iMaximumSize, float iStep)
    : SceneOptimization{iPriority}, maximumSize{iMaximumSize}, step{iStep}
{
}

TextureOptimization::~TextureOptimization() = default;

std::string TextureOptimization::getDescription() const
{
  return "Reducing render target texture size to " + std::to_string(maximumSize);
}

bool TextureOptimization::apply(Scene* scene, SceneOptimizer* /*optimizer*/)
{
  auto allDone = true;
  for (auto& texture : scene->textures) {
    if (!texture->canRescale()) {
      continue;
    }

    auto currentSize  = texture->getSize();
    auto maxDimension = std::max(currentSize.width, currentSize.height);

    if (maxDimension > maximumSize) {
      texture->scale(step);
      allDone = false;
    }
  }

  return allDone;
}

} // end of namespace BABYLON
