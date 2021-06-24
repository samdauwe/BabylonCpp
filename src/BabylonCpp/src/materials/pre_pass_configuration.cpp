#include <babylon/materials/pre_pass_configuration.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/effect.h>
#include <babylon/meshes/mesh.h>
#include <babylon/rendering/pre_pass_renderer.h>

namespace BABYLON {

PrePassConfiguration::PrePassConfiguration() : previousViewProjection{std::nullopt}
{
}

PrePassConfiguration::~PrePassConfiguration() = default;

void PrePassConfiguration::AddUniforms(std::vector<std::string>& uniforms)
{
  uniforms.emplace_back("previousWorld");
  uniforms.emplace_back("previousViewProjection");
}

void PrePassConfiguration::AddSamplers(std::vector<std::string>& /*samplers*/)
{
  // pass
}

void PrePassConfiguration::bindForSubMesh(const EffectPtr& effect, Scene* scene, Mesh* mesh,
                                          const Matrix& world, bool /*isFrozen*/)
{
  if (scene->prePassRenderer() && scene->prePassRenderer()->enabled()
      && scene->prePassRenderer()->currentRTisSceneRT()) {
    if (scene->prePassRenderer()->getIndex(Constants::PREPASS_VELOCITY_TEXTURE_TYPE) != -1) {
      if (!stl_util::contains(previousWorldMatrices, mesh->uniqueId)) {
        previousWorldMatrices[mesh->uniqueId] = Matrix::Identity();
      }

      if (!previousViewProjection) {
        previousViewProjection = scene->getTransformMatrix();
        currentViewProjection  = scene->getTransformMatrix();
      }

      if (currentViewProjection.updateFlag != scene->getTransformMatrix().updateFlag) {
        // First update of the prepass configuration for this rendering pass
        previousViewProjection->copyFrom(currentViewProjection);
        currentViewProjection.copyFrom(scene->getTransformMatrix());
        currentViewProjection.updateFlag = scene->getTransformMatrix().updateFlag;
      }

      effect->setMatrix("previousWorld", previousWorldMatrices[mesh->uniqueId]);
      effect->setMatrix("previousViewProjection", *previousViewProjection);

      previousWorldMatrices[mesh->uniqueId] = world;
    }
  }
}

} // end of namespace BABYLON
