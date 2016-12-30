#include <babylon/rendering/rendering_manager.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/rendering_group_info.h>
#include <babylon/mesh/sub_mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/rendering/rendering_group.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

RenderingManager::RenderingManager(Scene* scene)
    : _scene{scene}
    , _clearColor{Color4(0.f, 0.f, 0.f, 0.f)}
    , _renderinGroupInfo{nullptr}
{
  _autoClearDepthStencil.resize(MAX_RENDERINGGROUPS);
  _customOpaqueSortCompareFn.resize(MAX_RENDERINGGROUPS);
  _customAlphaTestSortCompareFn.resize(MAX_RENDERINGGROUPS);
  _customTransparentSortCompareFn.resize(MAX_RENDERINGGROUPS);

  for (unsigned int i = RenderingManager::MIN_RENDERINGGROUPS;
       i < RenderingManager::MAX_RENDERINGGROUPS; ++i) {
    _autoClearDepthStencil[i]          = true;
    _customOpaqueSortCompareFn[i]      = nullptr;
    _customAlphaTestSortCompareFn[i]   = nullptr;
    _customTransparentSortCompareFn[i] = nullptr;
  }
}

RenderingManager::~RenderingManager()
{
}

void RenderingManager::_renderParticles(
  unsigned int index, const std::vector<AbstractMesh*>& activeMeshes)
{
  if (_scene->_activeParticleSystems.empty()) {
    return;
  }

  // Particles
  auto activeCamera = _scene->activeCamera;
  _scene->_particlesDuration.beginMonitoring();
  for (auto& particleSystem : _scene->_activeParticleSystems) {
    if (particleSystem->renderingGroupId != index) {
      continue;
    }

    if ((activeCamera->layerMask & particleSystem->layerMask) == 0) {
      continue;
    }

    _clearDepthStencilBuffer();

    if (activeMeshes.empty()
        || (std::find(activeMeshes.begin(), activeMeshes.end(),
                      particleSystem->emitter)
            != activeMeshes.end())) {
      _scene->_activeParticles.addCount(particleSystem->render(), false);
    }
  }
  _scene->_particlesDuration.endMonitoring(false);
}

void RenderingManager::_renderSprites(unsigned int index)
{
  if (!_scene->spritesEnabled || _scene->spriteManagers.empty()) {
    return;
  }

  // Sprites
  auto activeCamera = _scene->activeCamera;
  _scene->_spritesDuration.beginMonitoring();
  for (auto& spriteManager : _scene->spriteManagers) {
    if (spriteManager->renderingGroupId == index
        && ((activeCamera->layerMask & spriteManager->layerMask) != 0)) {
      _clearDepthStencilBuffer();
      spriteManager->render();
    }
  }
  _scene->_spritesDuration.endMonitoring(false);
}

void RenderingManager::_clearDepthStencilBuffer()
{
  if (_depthBufferAlreadyCleaned) {
    return;
  }

  _scene->getEngine()->clear(_clearColor, false, true, true);
  _depthBufferAlreadyCleaned = true;
}

void RenderingManager::_renderSpritesAndParticles()
{
  if (_currentRenderSprites) {
    _renderSprites(_currentIndex);
  }

  if (_currentRenderParticles) {
    _renderParticles(_currentIndex, _currentActiveMeshes);
  }
}

void RenderingManager::render(
  std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                     const std::vector<SubMesh*>& transparentSubMeshes,
                     const std::vector<SubMesh*>& alphaTestSubMeshes)>
    customRenderFunction,
  const std::vector<AbstractMesh*>& activeMeshes, bool renderParticles,
  bool renderSprites)
{
  // Check if there's at least on observer on the onRenderingGroupObservable and
  // initialize things to fire it
  bool hasObservable = _scene->onRenderingGroupObservable.hasObservers();
  if (hasObservable) {
    if (!_renderinGroupInfo) {
      _renderinGroupInfo = std_util::make_unique<RenderingGroupInfo>();
    }
    _renderinGroupInfo->scene  = _scene;
    _renderinGroupInfo->camera = _scene->activeCamera;
  }

  _currentActiveMeshes    = activeMeshes;
  _currentRenderParticles = renderParticles;
  _currentRenderSprites   = renderSprites;

  auto info = _renderinGroupInfo.get();

  for (unsigned int index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    _depthBufferAlreadyCleaned
      = (index == RenderingManager::MIN_RENDERINGGROUPS);
    RenderingGroup* renderingGroup = (index < _renderingGroups.size()) ?
                                       _renderingGroups[index].get() :
                                       nullptr;
    bool needToStepBack = false;

    _currentIndex          = index;
    int renderingGroupMask = 0;

    if (renderingGroup) {
      // Fire PRECLEAR stage
      if (hasObservable) {
        renderingGroupMask     = static_cast<int>(std::pow(2, index));
        info->renderStage      = RenderingGroupInfo::STAGE_PRECLEAR;
        info->renderingGroupId = index;
        _scene->onRenderingGroupObservable.notifyObservers(info,
                                                           renderingGroupMask);
      }

      // Clear depth/stencil if needed
      if (_autoClearDepthStencil[index]) {
        _clearDepthStencilBuffer();
      }

      // Fire PREOPAQUE stage
      if (hasObservable) {
        info->renderStage = RenderingGroupInfo::STAGE_PREOPAQUE;
        _scene->onRenderingGroupObservable.notifyObservers(info,
                                                           renderingGroupMask);
      }

      if (!renderingGroup->onBeforeTransparentRendering) {
        renderingGroup->onBeforeTransparentRendering
          = [this]() { _renderSpritesAndParticles(); };
      }

      // Fire PRETRANSPARENT stage
      if (hasObservable) {
        info->renderStage = RenderingGroupInfo::STAGE_PRETRANSPARENT;
        _scene->onRenderingGroupObservable.notifyObservers(info,
                                                           renderingGroupMask);
      }

      if (!renderingGroup->render(customRenderFunction)) {
        _renderingGroups.erase(_renderingGroups.begin() + index);
        needToStepBack = true;
        _renderSpritesAndParticles();
      }

      // Fire POSTTRANSPARENT stage
      if (hasObservable) {
        info->renderStage = RenderingGroupInfo::STAGE_POSTTRANSPARENT;
        _scene->onRenderingGroupObservable.notifyObservers(info,
                                                           renderingGroupMask);
      }
    }
    else {
      _renderSpritesAndParticles();

      if (hasObservable) {
        renderingGroupMask     = static_cast<int>(std::pow(2, index));
        info->renderStage      = RenderingGroupInfo::STAGE_PRECLEAR;
        info->renderingGroupId = index;
        _scene->onRenderingGroupObservable.notifyObservers(info,
                                                           renderingGroupMask);

        info->renderStage = RenderingGroupInfo::STAGE_POSTTRANSPARENT;
        _scene->onRenderingGroupObservable.notifyObservers(info,
                                                           renderingGroupMask);
      }
    }

    if (needToStepBack) {
      --index;
    }
  }
}

void RenderingManager::reset()
{
  for (unsigned index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    RenderingGroup* renderingGroup = (index < _renderingGroups.size()) ?
                                       _renderingGroups[index].get() :
                                       nullptr;
    if (renderingGroup) {
      renderingGroup->prepare();
    }
  }
}

void RenderingManager::dispatch(SubMesh* subMesh)
{
  auto mesh             = subMesh->getMesh();
  auto renderingGroupId = mesh->renderingGroupId;

  // Resize render groups vector if needed
  if (renderingGroupId >= _renderingGroups.size()) {
    for (size_t i = _renderingGroups.size(); i <= renderingGroupId; ++i) {
      _renderingGroups.emplace_back(nullptr);
    }
  }

  if (!_renderingGroups[renderingGroupId]) {
    _renderingGroups[renderingGroupId]
      = std::unique_ptr<RenderingGroup>(new RenderingGroup(
        renderingGroupId, _scene, _customOpaqueSortCompareFn[renderingGroupId],
        _customAlphaTestSortCompareFn[renderingGroupId],
        _customTransparentSortCompareFn[renderingGroupId]));
  }

  _renderingGroups[renderingGroupId]->dispatch(subMesh);
}

void RenderingManager::setRenderingOrder(
  unsigned int renderingGroupId,
  const std::function<int(SubMesh* a, SubMesh* b)>& opaqueSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& alphaTestSortCompareFn,
  const std::function<int(SubMesh* a, SubMesh* b)>& transparentSortCompareFn)
{
  _customOpaqueSortCompareFn[renderingGroupId]      = opaqueSortCompareFn;
  _customAlphaTestSortCompareFn[renderingGroupId]   = alphaTestSortCompareFn;
  _customTransparentSortCompareFn[renderingGroupId] = transparentSortCompareFn;

  if (_renderingGroups[renderingGroupId]) {
    auto& group = _renderingGroups[renderingGroupId];
    group->setOpaqueSortCompareFn(_customOpaqueSortCompareFn[renderingGroupId]);
    group->setAlphaTestSortCompareFn(
      _customAlphaTestSortCompareFn[renderingGroupId]);
    group->setTransparentSortCompareFn(
      _customTransparentSortCompareFn[renderingGroupId]);
  }
}

void RenderingManager::setRenderingAutoClearDepthStencil(
  unsigned int renderingGroupId, bool autoClearDepthStencil)
{
  _autoClearDepthStencil[renderingGroupId] = autoClearDepthStencil;
}

} // end of namespace BABYLON
