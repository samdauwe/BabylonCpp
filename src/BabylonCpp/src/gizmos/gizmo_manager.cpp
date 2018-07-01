#include <babylon/gizmos/gizmo_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/scene.h>
#include <babylon/gizmos/position_gizmo.h>
#include <babylon/gizmos/rotation_gizmo.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

GizmoManager::GizmoManager(Scene* iScene, Nullable<GizmoManagerOptions> options)
    : _scene{iScene}, _gizmoLayer{nullptr}, _pointerObserver{nullptr}
{
  _gizmoLayer = ::std::make_unique<UtilityLayerRenderer>(_scene);

  // Options parsing
  if (!options) {
    options = GizmoManagerOptions{};
  }
  if ((*options).singleGizmo.isNull()) {
    auto _options        = *options;
    _options.singleGizmo = true;
    options              = _options;
  }

  // Instatiate/dispose gizmos based on pointer actions
  _pointerObserver = _scene->onPointerObservable.add(
    [&, options](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (pointerInfo->type == PointerEventTypes::POINTERDOWN) {
        if (pointerInfo->pickInfo.pickedMesh) {
          if (!stl_util::contains(_gizmoSet,
                                  pointerInfo->pickInfo.pickedMesh->uniqueId)) {
            if ((*options).singleGizmo) {
              _clearGizmos();
            }
            // Enable gizmo when mesh is selected
            _gizmoSet[pointerInfo->pickInfo.pickedMesh->uniqueId] = {
              ::std::make_unique<PositionGizmo>(
                _gizmoLayer.get()), // positionGizmo
              ::std::make_unique<RotationGizmo>(
                _gizmoLayer.get()) // rotationGizmo
            };
            _gizmoSet[pointerInfo->pickInfo.pickedMesh->uniqueId]
              .positionGizmo->attachedMesh
              = pointerInfo->pickInfo.pickedMesh;
            _gizmoSet[pointerInfo->pickInfo.pickedMesh->uniqueId]
              .rotationGizmo->attachedMesh
              = pointerInfo->pickInfo.pickedMesh;
          }
          else {
            if (!(*options).singleGizmo) {
              // Disable gizmo when clicked again
              _gizmoSet[pointerInfo->pickInfo.pickedMesh->uniqueId]
                .positionGizmo->dispose();
              _gizmoSet[pointerInfo->pickInfo.pickedMesh->uniqueId]
                .rotationGizmo->dispose();
              _gizmoSet.erase(pointerInfo->pickInfo.pickedMesh->uniqueId);
            }
          }
        }
        else {
          if ((*options).singleGizmo) {
            // Disable gizmo when clicked away
            if (pointerInfo->pickInfo.ray) {
              auto gizmoPick = _gizmoLayer->utilityLayerScene->pickWithRay(
                *pointerInfo->pickInfo.ray);
              if (gizmoPick && !(*gizmoPick).hit) {
                _clearGizmos();
              }
            }
          }
        }
      }
    });
}

GizmoManager::~GizmoManager()
{
}

void GizmoManager::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _scene->onPointerObservable.remove(_pointerObserver);
  _clearGizmos();
  _gizmoLayer->dispose(doNotRecurse, disposeMaterialAndTextures);
}

void GizmoManager::_clearGizmos()
{
  for (auto& item : _gizmoSet) {
    auto& value = item.second;
    if (value.positionGizmo) {
      value.positionGizmo->dispose();
    }
    if (value.rotationGizmo) {
      value.rotationGizmo->dispose();
    }
  }
  _gizmoSet.clear();
}

} // end of namespace BABYLON
