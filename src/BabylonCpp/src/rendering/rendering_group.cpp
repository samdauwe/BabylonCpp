#include <babylon/rendering/rendering_group.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/culling/bounding_sphere.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/rendering/edges_renderer.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

RenderingGroup::RenderingGroup(
  unsigned int iIndex, Scene* scene,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn)
    : index{iIndex}
    , onBeforeTransparentRendering{nullptr}
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

  setOpaqueSortCompareFn(opaqueSortCompareFn);
  setAlphaTestSortCompareFn(alphaTestSortCompareFn);
  setTransparentSortCompareFn(transparentSortCompareFn);
}

RenderingGroup::~RenderingGroup()
{
}

void RenderingGroup::setOpaqueSortCompareFn(
  const ::std::function<int(SubMesh* a, SubMesh* b)>& value)
{
  _opaqueSortCompareFn = value;
  if (value) {
    _renderOpaque = [this](const vector_t<SubMesh*>& subMeshes) {
      renderOpaqueSorted(subMeshes);
    };
  }
  else {
    _renderOpaque = [](const vector_t<SubMesh*>& subMeshes) {
      RenderingGroup::renderUnsorted(subMeshes);
    };
  }
}

void RenderingGroup::setAlphaTestSortCompareFn(
  const ::std::function<int(SubMesh* a, SubMesh* b)>& value)
{
  _alphaTestSortCompareFn = value;
  if (value) {
    _renderAlphaTest = [this](const vector_t<SubMesh*>& subMeshes) {
      renderAlphaTestSorted(subMeshes);
    };
  }
  else {
    _renderAlphaTest = [](const vector_t<SubMesh*>& subMeshes) {
      RenderingGroup::renderUnsorted(subMeshes);
    };
  }
}

void RenderingGroup::setTransparentSortCompareFn(
  const ::std::function<int(SubMesh* a, SubMesh* b)>& value)
{
  if (value) {
    _transparentSortCompareFn = value;
  }
  else {
    _transparentSortCompareFn = [](SubMesh* a, SubMesh* b) {
      return RenderingGroup::defaultTransparentSortCompare(a, b);
    };
  }
  _renderTransparent = [this](const vector_t<SubMesh*>& subMeshes) {
    renderTransparentSorted(subMeshes);
  };
}

void RenderingGroup::render(
  ::std::function<void(const vector_t<SubMesh*>& opaqueSubMeshes,
                       const vector_t<SubMesh*>& alphaTestSubMeshes,
                       const vector_t<SubMesh*>& transparentSubMeshes,
                       const vector_t<SubMesh*>& depthOnlySubMeshes,
                       const ::std::function<void()>& beforeTransparents)>&
    customRenderFunction,
  bool renderSprites, bool renderParticles,
  const vector_t<AbstractMesh*> activeMeshes)
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
    engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
  }

  // Set back stencil to false in case it changes before the edge renderer.
  engine->setStencilBuffer(false);

  // Edges
  for (auto& edgesRenderer : _edgesRenderers) {
    edgesRenderer->render();
  }

  // Restore Stencil state.
  engine->setStencilBuffer(stencilState);
}

void RenderingGroup::renderOpaqueSorted(const vector_t<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _opaqueSortCompareFn,
                                      _scene->activeCamera, false);
}

void RenderingGroup::renderAlphaTestSorted(const vector_t<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _alphaTestSortCompareFn,
                                      _scene->activeCamera, false);
}

void RenderingGroup::renderTransparentSorted(
  const vector_t<SubMesh*>& subMeshes)
{
  return RenderingGroup::renderSorted(subMeshes, _transparentSortCompareFn,
                                      _scene->activeCamera, true);
}

void RenderingGroup::renderSorted(
  const vector_t<SubMesh*>& subMeshes,
  const ::std::function<int(SubMesh* a, SubMesh* b)>& sortCompareFn,
  Camera* camera, bool transparent)
{
  auto cameraPosition = camera ? camera->globalPosition() : Vector3::Zero();
  for (auto& subMesh : subMeshes) {
    subMesh->_alphaIndex = subMesh->getMesh()->alphaIndex;
    subMesh->_distanceToCamera
      = subMesh->getBoundingInfo()
          .boundingSphere.centerWorld.subtract(cameraPosition)
          .length();
  }

  auto sortedArray = subMeshes;

  // sort using a custom function object
  if (sortCompareFn) {
    ::std::sort(
      sortedArray.begin(), sortedArray.end(),
      [&sortCompareFn](SubMesh* a, SubMesh* b) { return sortCompareFn(a, b); });
  }

  for (auto& subMesh : sortedArray) {
    if (transparent) {
      auto material = subMesh->getMaterial();

      if (material && material->needDepthPrePass()) {
        auto engine = material->getScene()->getEngine();
        engine->setColorWrite(false);
        engine->setAlphaMode(EngineConstants::ALPHA_DISABLE);
        subMesh->render(false);
        engine->setColorWrite(true);
      }
    }

    subMesh->render(transparent);
  }
}

void RenderingGroup::renderUnsorted(const vector_t<SubMesh*>& subMeshes)
{
  for (auto& subMesh : subMeshes) {
    subMesh->render(false);
  }
}

int RenderingGroup::defaultTransparentSortCompare(SubMesh* a, SubMesh* b)
{
  // Alpha index first
  if (a->_alphaIndex > b->_alphaIndex) {
    return 1;
  }
  if (a->_alphaIndex < b->_alphaIndex) {
    return -1;
  }

  // Then distance to camera
  return RenderingGroup::backToFrontSortCompare(a, b);
}

int RenderingGroup::backToFrontSortCompare(SubMesh* a, SubMesh* b)
{
  // Then distance to camera
  if (a->_distanceToCamera < b->_distanceToCamera) {
    return 1;
  }
  if (a->_distanceToCamera > b->_distanceToCamera) {
    return -1;
  }

  return 0;
}

int RenderingGroup::frontToBackSortCompare(SubMesh* a, SubMesh* b)
{
  // Then distance to camera
  if (a->_distanceToCamera < b->_distanceToCamera) {
    return -1;
  }
  if (a->_distanceToCamera > b->_distanceToCamera) {
    return 1;
  }

  return 0;
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

void RenderingGroup::dispatch(SubMesh* subMesh, AbstractMesh* iMesh,
                              Material* iMaterial)
{
  // Get mesh and materials if not provided
  auto mesh = iMesh;
  if (!mesh) {
    mesh = subMesh->getMesh();
  }

  auto material = iMaterial;
  if (!material) {
    material = subMesh->getMaterial();
  }

  if (!material) {
    return;
  }

  if (material->needAlphaBlendingForMesh(mesh)) { // Transparent
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

  if (mesh->_edgesRenderer) {
    _edgesRenderers.emplace_back(mesh->_edgesRenderer.get());
  }
}

void RenderingGroup::dispatchSprites(SpriteManager* spriteManager)
{
  _spriteManagers.emplace_back(spriteManager);
}

void RenderingGroup::dispatchParticles(IParticleSystem* particleSystem)
{
  _particleSystems.emplace_back(particleSystem);
}

void RenderingGroup::_renderParticles(
  const vector_t<AbstractMesh*>& activeMeshes)
{
  if (_particleSystems.empty()) {
    return;
  }

  // Particles
  auto& activeCamera = _scene->activeCamera;
  _scene->onBeforeParticlesRenderingObservable.notifyObservers(_scene);
  for (auto& particleSystem : _particleSystems) {
    if ((activeCamera && activeCamera->layerMask & particleSystem->layerMask)
        == 0) {
      continue;
    }
    if (!activeMeshes.empty()
        || (particleSystem->emitter.is<AbstractMesh*>()
            && stl_util::index_of(activeMeshes,
                                  particleSystem->emitter.get<AbstractMesh*>())
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
  auto& activeCamera = _scene->activeCamera;
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
