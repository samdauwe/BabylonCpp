#include <babylon/rendering/rendering_group.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engines/constants.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

RenderingGroup::RenderingGroup(
  unsigned int iIndex, Scene* scene,
  const std::function<bool(const SubMesh* a, const SubMesh* b)>&
    iOpaqueSortCompareFn,
  const std::function<bool(const SubMesh* a, const SubMesh* b)>&
    iAlphaTestSortCompareFn,
  const std::function<bool(const SubMesh* a, const SubMesh* b)>&
    iTransparentSortCompareFn)
    : index{iIndex}
    , onBeforeTransparentRendering{nullptr}
    , opaqueSortCompareFn{this, &RenderingGroup::set_opaqueSortCompareFn}
    , alphaTestSortCompareFn{this, &RenderingGroup::set_alphaTestSortCompareFn}
    , transparentSortCompareFn{this,
                               &RenderingGroup::set_transparentSortCompareFn}
    , _scene{scene}
    , _opaqueSortCompareFn{nullptr}
    , _alphaTestSortCompareFn{nullptr}
    , _transparentSortCompareFn{nullptr}
    , _renderOpaque{nullptr}
    , _renderAlphaTest{nullptr}
    , _renderTransparent{nullptr}
{
  _opaqueSubMeshes.reserve(256);
  _transparentSubMeshes.reserve(256);
  _alphaTestSubMeshes.reserve(256);
  _depthOnlySubMeshes.reserve(256);
  _particleSystems.reserve(256);
  _spriteManagers.reserve(256);

  _edgesRenderers.reserve(16);

  opaqueSortCompareFn      = iOpaqueSortCompareFn;
  alphaTestSortCompareFn   = iAlphaTestSortCompareFn;
  transparentSortCompareFn = iTransparentSortCompareFn;
}

RenderingGroup::~RenderingGroup()
{
}

void RenderingGroup::set_opaqueSortCompareFn(
  const std::function<bool(const SubMesh* a, const SubMesh* b)>& value)
{
  _opaqueSortCompareFn = value;
  if (value) {
    _renderOpaque = [this](const std::vector<SubMesh*>& subMeshes) {
      renderOpaqueSorted(subMeshes);
    };
  }
  else {
    _renderOpaque = [](const std::vector<SubMesh*>& subMeshes) {
      RenderingGroup::renderUnsorted(subMeshes);
    };
  }
}

void RenderingGroup::set_alphaTestSortCompareFn(
  const std::function<bool(const SubMesh* a, const SubMesh* b)>& value)
{
  _alphaTestSortCompareFn = value;
  if (value) {
    _renderAlphaTest = [this](const std::vector<SubMesh*>& subMeshes) {
      renderAlphaTestSorted(subMeshes);
    };
  }
  else {
    _renderAlphaTest = [](const std::vector<SubMesh*>& subMeshes) {
      RenderingGroup::renderUnsorted(subMeshes);
    };
  }
}

void RenderingGroup::set_transparentSortCompareFn(
  const std::function<bool(const SubMesh* a, const SubMesh* b)>& value)
{
  if (value) {
    _transparentSortCompareFn = value;
  }
  else {
    _transparentSortCompareFn = [](const SubMesh* a, const SubMesh* b) {
      return RenderingGroup::defaultTransparentSortCompare(a, b);
    };
  }
  _renderTransparent = [this](const std::vector<SubMesh*>& subMeshes) {
    renderTransparentSorted(subMeshes);
  };
}

void RenderingGroup::render(
  std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                     const std::vector<SubMesh*>& alphaTestSubMeshes,
                     const std::vector<SubMesh*>& transparentSubMeshes,
                     const std::vector<SubMesh*>& depthOnlySubMeshes,
                     const std::function<void()>& beforeTransparents)>&
    customRenderFunction,
  bool renderSprites, bool renderParticles,
  const std::vector<AbstractMesh*>& activeMeshes)
{
  if (customRenderFunction) {
    customRenderFunction(_opaqueSubMeshes, _alphaTestSubMeshes,
                         _transparentSubMeshes, _depthOnlySubMeshes, nullptr);
    return;
  }

  auto engine = _scene->getEngine();

  // Depth only
  if (!_depthOnlySubMeshes.empty()) {
    engine->setColorWrite(false);
    _renderAlphaTest(_depthOnlySubMeshes);
    engine->setColorWrite(true);
  }

  // Opaque
  if (!_opaqueSubMeshes.empty()) {
    _renderOpaque(_opaqueSubMeshes);
  }

  // Alpha test
  if (!_alphaTestSubMeshes.empty()) {
    _renderAlphaTest(_alphaTestSubMeshes);
  }

  auto stencilState = engine->getStencilBuffer();
  engine->setStencilBuffer(false);

  // Sprites
  if (renderSprites) {
    _renderSprites();
  }

  // Particles
  if (renderParticles) {
    _renderParticles(activeMeshes);
  }

  if (onBeforeTransparentRendering) {
    onBeforeTransparentRendering();
  }

  // Transparent
  if (!_transparentSubMeshes.empty()) {
    _renderTransparent(_transparentSubMeshes);
    engine->setAlphaMode(Constants::ALPHA_DISABLE);
  }

  // Set back stencil to false in case it changes before the edge renderer.
  engine->setStencilBuffer(false);

  // Edges
  if (!_edgesRenderers.empty()) {
    for (auto& edgesRenderer : _edgesRenderers) {
      edgesRenderer->render();
    }

    engine->setAlphaMode(Constants::ALPHA_DISABLE);
  }

  // Restore Stencil state.
  engine->setStencilBuffer(stencilState);
}

void RenderingGroup::renderOpaqueSorted(const std::vector<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _opaqueSortCompareFn,
                                      _scene->activeCamera(), false);
}

void RenderingGroup::renderAlphaTestSorted(
  const std::vector<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _alphaTestSortCompareFn,
                                      _scene->activeCamera(), false);
}

void RenderingGroup::renderTransparentSorted(
  const std::vector<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _transparentSortCompareFn,
                                      _scene->activeCamera(), true);
}

void RenderingGroup::renderSorted(
  const std::vector<SubMesh*>& subMeshes,
  const std::function<bool(const SubMesh* a, const SubMesh* b)>& sortCompareFn,
  const CameraPtr& camera, bool transparent)
{
  auto cameraPosition = camera ? camera->globalPosition() : Vector3::Zero();
  for (auto& subMesh : subMeshes) {
    subMesh->_alphaIndex = subMesh->getMesh()->alphaIndex;
    subMesh->_distanceToCamera
      = subMesh->getBoundingInfo()
          ->boundingSphere.centerWorld.subtract(cameraPosition)
          .length();
  }

  auto sortedArray = subMeshes;

  // sort using a custom function object
  if (sortCompareFn) {
    std::stable_sort(sortedArray.begin(), sortedArray.end(), sortCompareFn);
  }

  for (auto& subMesh : sortedArray) {
    if (transparent) {
      auto material = subMesh->getMaterial();

      if (material && material->needDepthPrePass()) {
        auto engine = material->getScene()->getEngine();
        engine->setColorWrite(false);
        engine->setAlphaMode(Constants::ALPHA_DISABLE);
        subMesh->render(false);
        engine->setColorWrite(true);
      }
    }

    subMesh->render(transparent);
  }
}

void RenderingGroup::renderUnsorted(const std::vector<SubMesh*>& subMeshes)
{
  for (auto& subMesh : subMeshes) {
    subMesh->render(false);
  }
}

bool RenderingGroup::defaultTransparentSortCompare(const SubMesh* a,
                                                  const SubMesh* b)
{
  // Alpha index first
  if (a->_alphaIndex > b->_alphaIndex) {
    return true;
  }
  if (a->_alphaIndex < b->_alphaIndex) {
    return false;
  }

  // Then distance to camera
  return RenderingGroup::backToFrontSortCompare(a, b);
}

bool RenderingGroup::backToFrontSortCompare(const SubMesh* a, const SubMesh* b)
{
  // Then distance to camera
  if (a->_distanceToCamera < b->_distanceToCamera) {
    return true;
  }
  if (a->_distanceToCamera > b->_distanceToCamera) {
    return false;
  }

  return false;
}

bool RenderingGroup::frontToBackSortCompare(SubMesh* a, SubMesh* b)
{
  // Then distance to camera
  if (a->_distanceToCamera < b->_distanceToCamera) {
    return false;
  }
  if (a->_distanceToCamera > b->_distanceToCamera) {
    return true;
  }

  return false;
}

void RenderingGroup::prepare()
{
  _opaqueSubMeshes.clear();
  _transparentSubMeshes.clear();
  _alphaTestSubMeshes.clear();
  _depthOnlySubMeshes.clear();
  _particleSystems.clear();
  _spriteManagers.clear();
  _edgesRenderers.clear();
}

void RenderingGroup::dispose()
{
  _opaqueSubMeshes.clear();
  _transparentSubMeshes.clear();
  _alphaTestSubMeshes.clear();
  _depthOnlySubMeshes.clear();
  _particleSystems.clear();
  _spriteManagers.clear();
  _edgesRenderers.clear();
}

void RenderingGroup::dispatch(SubMesh* subMesh, AbstractMesh* mesh,
                              MaterialPtr material)
{
  // Get mesh and materials if not provided
  if (!mesh) {
    mesh = subMesh->getMesh().get();
  }

  if (!material) {
    material = subMesh->getMaterial();
  }

  if (!material) {
    return;
  }

  if (material->needAlphaBlendingForMesh(*mesh)) { // Transparent
    _transparentSubMeshes.emplace_back(subMesh);
  }
  else if (material->needAlphaTesting()) { // Alpha test
    if (material->needDepthPrePass()) {
      _depthOnlySubMeshes.emplace_back(subMesh);
    }
    _alphaTestSubMeshes.emplace_back(subMesh);
  }
  else {
    if (material->needDepthPrePass()) {
      _depthOnlySubMeshes.emplace_back(subMesh);
    }
    _opaqueSubMeshes.emplace_back(subMesh); // Opaque
  }

  mesh->_renderingGroup = this;

  if (mesh->_edgesRenderer != nullptr && mesh->_edgesRenderer->isEnabled) {
    _edgesRenderers.emplace_back(mesh->_edgesRenderer.get());
  }
}

void RenderingGroup::dispatchSprites(ISpriteManager* spriteManager)
{
  _spriteManagers.emplace_back(spriteManager);
}

void RenderingGroup::dispatchParticles(IParticleSystem* particleSystem)
{
  _particleSystems.emplace_back(particleSystem);
}

void RenderingGroup::_renderParticles(
  const std::vector<AbstractMesh*>& activeMeshes)
{
  if (_particleSystems.empty()) {
    return;
  }

  // Particles
  const auto& activeCamera = _scene->activeCamera();
  _scene->onBeforeParticlesRenderingObservable.notifyObservers(_scene);
  for (auto& particleSystem : _particleSystems) {
    if ((activeCamera && activeCamera->layerMask & particleSystem->layerMask)
        == 0) {
      continue;
    }
    if (!activeMeshes.empty()
        || (std::holds_alternative<AbstractMeshPtr>(particleSystem->emitter)
            && stl_util::index_of(
                 activeMeshes,
                 std::get<AbstractMeshPtr>(particleSystem->emitter).get())
                 != -1)) {
      _scene->_activeParticles.addCount(particleSystem->render(), false);
    }
  }
  _scene->onAfterParticlesRenderingObservable.notifyObservers(_scene);
}

void RenderingGroup::_renderSprites()
{
  if (!_scene->spritesEnabled || _spriteManagers.empty()) {
    return;
  }

  // Sprites
  auto& activeCamera = _scene->activeCamera();
  _scene->onBeforeSpritesRenderingObservable.notifyObservers(_scene);
  for (auto& spriteManager : _spriteManagers) {
    if (((activeCamera && activeCamera->layerMask & spriteManager->layerMask)
         != 0)) {
      spriteManager->render();
    }
  }
  _scene->onAfterSpritesRenderingObservable.notifyObservers(_scene);
}

} // end of namespace BABYLON
