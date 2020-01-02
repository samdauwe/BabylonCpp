#include <babylon/rendering/rendering_manager.h>

#include <babylon/cameras/camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/rendering_group_info.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/material.h>
#include <babylon/meshes/sub_mesh.h>
#include <babylon/particles/particle_system.h>
#include <babylon/rendering/irendering_manager_auto_clear_setup.h>
#include <babylon/rendering/rendering_group.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

bool RenderingManager::AUTOCLEAR = true;

RenderingManager::RenderingManager(Scene* scene)
    : _useSceneAutoClearSetup{false}
    , _scene{scene}
    , _renderingGroupInfo{std::make_unique<RenderingGroupInfo>()}
{
  _autoClearDepthStencil.resize(MAX_RENDERINGGROUPS);
  _customOpaqueSortCompareFn.resize(MAX_RENDERINGGROUPS);
  _customAlphaTestSortCompareFn.resize(MAX_RENDERINGGROUPS);
  _customTransparentSortCompareFn.resize(MAX_RENDERINGGROUPS);

  for (unsigned int i = RenderingManager::MIN_RENDERINGGROUPS;
       i < RenderingManager::MAX_RENDERINGGROUPS; ++i) {
    _autoClearDepthStencil[i]          = {true, true, true};
    _customOpaqueSortCompareFn[i]      = nullptr;
    _customAlphaTestSortCompareFn[i]   = nullptr;
    _customTransparentSortCompareFn[i] = nullptr;
  }
}

RenderingManager::~RenderingManager() = default;

void RenderingManager::_clearDepthStencilBuffer(bool depth, bool stencil)
{
  if (_depthStencilBufferAlreadyCleaned) {
    return;
  }

  _scene->getEngine()->clear(std::nullopt, false, depth, stencil);
  _depthStencilBufferAlreadyCleaned = true;
}

void RenderingManager::render(std::function<void(const std::vector<SubMesh*>& opaqueSubMeshes,
                                                 const std::vector<SubMesh*>& alphaTestSubMeshes,
                                                 const std::vector<SubMesh*>& transparentSubMeshes,
                                                 const std::vector<SubMesh*>& depthOnlySubMeshes,
                                                 const std::function<void()>& beforeTransparents)>
                                customRenderFunction,
                              const std::vector<AbstractMesh*>& activeMeshes, bool renderParticles,
                              bool renderSprites)
{
  // Update the observable context (not null as it only goes away on dispose)
  auto& info   = _renderingGroupInfo;
  info->scene  = _scene;
  info->camera = _scene->activeCamera;

  // Dispatch sprites
  if (!_scene->spriteManagers.empty() && renderSprites) {
    for (const auto& manager : _scene->spriteManagers) {
      dispatchSprites(manager.get());
    }
  }

  // Render
  for (unsigned int index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    _depthStencilBufferAlreadyCleaned = (index == RenderingManager::MIN_RENDERINGGROUPS);
    RenderingGroup* renderingGroup
      = (index < _renderingGroups.size()) ? _renderingGroups[index].get() : nullptr;
    if (!renderingGroup) {
      continue;
    }

    auto renderingGroupMask = static_cast<int>(std::pow(2, index));
    info->renderingGroupId  = index;

    // Before Observable
    _scene->onBeforeRenderingGroupObservable.notifyObservers(info.get(), renderingGroupMask);

    // Clear depth/stencil if needed
    if (RenderingManager::AUTOCLEAR) {
      if (_useSceneAutoClearSetup) {
        const auto autoClear = _scene->getAutoClearDepthStencilSetup(index);

        if (autoClear.has_value() && (*autoClear).autoClear) {
          _clearDepthStencilBuffer((*autoClear).depth, (*autoClear).stencil);
        }
      }
      else {
        if (index < _autoClearDepthStencil.size()) {
          auto autoClear = _autoClearDepthStencil[index];
          if (autoClear.autoClear) {
            _clearDepthStencilBuffer(autoClear.depth, autoClear.stencil);
          }
        }
      }
    }

    // Render
    const auto iIndex = static_cast<int>(index);
    for (const auto& step : _scene->_beforeRenderingGroupDrawStage) {
      step.action(iIndex);
    }
    renderingGroup->render(customRenderFunction, renderSprites, renderParticles, activeMeshes);
    for (const auto& step : _scene->_afterRenderingGroupDrawStage) {
      step.action(iIndex);
    }

    // After Observable
    _scene->onAfterRenderingGroupObservable.notifyObservers(info.get(), renderingGroupMask);
  }
}

void RenderingManager::reset()
{
  for (unsigned index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    if (index < _renderingGroups.size()) {
      auto& renderingGroup = _renderingGroups[index];
      if (renderingGroup) {
        renderingGroup->prepare();
      }
    }
  }
}

void RenderingManager::dispose()
{
  freeRenderingGroups();
  _renderingGroups.clear();
  _renderingGroupInfo = nullptr;
}

void RenderingManager::freeRenderingGroups()
{
  for (unsigned index = RenderingManager::MIN_RENDERINGGROUPS;
       index < RenderingManager::MAX_RENDERINGGROUPS; ++index) {
    if (index < _renderingGroups.size()) {
      auto& renderingGroup = _renderingGroups[index];
      if (renderingGroup) {
        renderingGroup->dispose();
      }
    }
  }
}

void RenderingManager::_prepareRenderingGroup(unsigned int renderingGroupId)
{
  // Resize render groups vector if needed
  if (renderingGroupId >= _renderingGroups.size()) {
    for (size_t i = _renderingGroups.size(); i <= renderingGroupId; ++i) {
      _renderingGroups.emplace_back(nullptr);
    }
  }

  if (!_renderingGroups[renderingGroupId]) {
    _renderingGroups[renderingGroupId] = std::make_unique<RenderingGroup>(
      renderingGroupId, _scene, _customOpaqueSortCompareFn[renderingGroupId],
      _customAlphaTestSortCompareFn[renderingGroupId],
      _customTransparentSortCompareFn[renderingGroupId]);
  }
}

void RenderingManager::dispatchSprites(ISpriteManager* spriteManager)
{
  const auto& renderingGroupId = spriteManager->renderingGroupId;

  _prepareRenderingGroup(renderingGroupId);

  _renderingGroups[renderingGroupId]->dispatchSprites(spriteManager);
}

void RenderingManager::dispatchParticles(IParticleSystem* particleSystem)
{
  const auto& renderingGroupId = particleSystem->renderingGroupId;

  _prepareRenderingGroup(renderingGroupId);

  _renderingGroups[renderingGroupId]->dispatchParticles(particleSystem);
}

void RenderingManager::dispatch(SubMesh* subMesh, AbstractMesh* mesh, const MaterialPtr& material)
{
  if (!mesh) {
    mesh = subMesh->getMesh().get();
  }
  const auto& renderingGroupId = static_cast<size_t>(mesh->renderingGroupId);

  _prepareRenderingGroup(static_cast<unsigned>(renderingGroupId));

  _renderingGroups[renderingGroupId]->dispatch(subMesh, mesh, material);
}

void RenderingManager::setRenderingOrder(
  unsigned int renderingGroupId,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& opaqueSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& alphaTestSortCompareFn,
  const std::function<int(const SubMesh* a, const SubMesh* b)>& transparentSortCompareFn)
{
  _customOpaqueSortCompareFn[renderingGroupId]      = opaqueSortCompareFn;
  _customAlphaTestSortCompareFn[renderingGroupId]   = alphaTestSortCompareFn;
  _customTransparentSortCompareFn[renderingGroupId] = transparentSortCompareFn;

  if (_renderingGroups[renderingGroupId]) {
    auto& group                     = _renderingGroups[renderingGroupId];
    group->opaqueSortCompareFn      = _customOpaqueSortCompareFn[renderingGroupId];
    group->alphaTestSortCompareFn   = _customAlphaTestSortCompareFn[renderingGroupId];
    group->transparentSortCompareFn = _customTransparentSortCompareFn[renderingGroupId];
  }
}

void RenderingManager::setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                                         bool autoClearDepthStencil, bool depth,
                                                         bool stencil)
{
  _autoClearDepthStencil[renderingGroupId] = {
    autoClearDepthStencil, // autoClear
    depth,                 // depth
    stencil                // stencil
  };
}

std::optional<IRenderingManagerAutoClearSetup>
RenderingManager::getAutoClearDepthStencilSetup(size_t index)
{
  if (index >= _autoClearDepthStencil.size()) {
    return std::nullopt;
  }

  return _autoClearDepthStencil[index];
}

} // end of namespace BABYLON
