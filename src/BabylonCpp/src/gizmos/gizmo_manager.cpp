#include <babylon/gizmos/gizmo_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/mesh/six_dof_drag_behavior.h>
#include <babylon/engine/scene.h>
#include <babylon/gizmos/bounding_box_gizmo.h>
#include <babylon/gizmos/position_gizmo.h>
#include <babylon/gizmos/rotation_gizmo.h>
#include <babylon/gizmos/scale_gizmo.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

GizmoManager::GizmoManager(Scene* iScene)
    : boundingBoxDragBehavior{std::make_unique<SixDofDragBehavior>()}
    , attachableMeshes{std::nullopt}
    , usePointerToAttachGizmos{true}
    , positionGizmoEnabled{this, &GizmoManager::get_positionGizmoEnabled,
                           &GizmoManager::set_positionGizmoEnabled}
    , rotationGizmoEnabled{this, &GizmoManager::get_rotationGizmoEnabled,
                           &GizmoManager::set_rotationGizmoEnabled}
    , scaleGizmoEnabled{this, &GizmoManager::get_scaleGizmoEnabled,
                        &GizmoManager::set_scaleGizmoEnabled}
    , boundingBoxGizmoEnabled{this, &GizmoManager::get_boundingBoxGizmoEnabled,
                              &GizmoManager::set_boundingBoxGizmoEnabled}
    , _scene{iScene}
    , _gizmosEnabled{false, false, false, false}
    , _pointerObserver{nullptr}
    , _attachedMesh{nullptr}
    , _boundingBoxColor{Color3::FromHexString("#0984e3")}
{
  _defaultKeepDepthUtilityLayer = UtilityLayerRenderer::New(iScene);
  _defaultKeepDepthUtilityLayer->utilityLayerScene->autoClearDepthAndStencil
    = false;
  _defaultUtilityLayer = UtilityLayerRenderer::New(iScene);

  // Instatiate/dispose gizmos based on pointer actions
  _pointerObserver = _scene->onPointerObservable.add(
    [this](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (!usePointerToAttachGizmos) {
        return;
      }
      if (pointerInfo->type == PointerEventTypes::POINTERDOWN) {
        if (pointerInfo->pickInfo.pickedMesh) {
          Node* node = pointerInfo->pickInfo.pickedMesh ?
                         pointerInfo->pickInfo.pickedMesh :
                         nullptr;
          if (attachableMeshes == std::nullopt) {
            // Attach to the most parent node
            while (node && node->parent() != nullptr) {
              // node = node->parent();
            }
          }
          else {
            // Attach to the parent node that is an attachableMesh
            auto found = false;
            for (auto& mesh : *attachableMeshes) {
              if (node
                  && (node == mesh.get() || node->isDescendantOf(mesh.get()))) {
                node  = mesh.get();
                found = true;
              }
            }
            if (!found) {
              node = nullptr;
            }
          }
          if (node && node->type() == Type::ABSTRACTMESH) {
            if (_attachedMesh != node) {
              attachToMesh(static_cast<AbstractMesh*>(node));
            }
          }
          else {
            attachToMesh(nullptr);
          }
        }
        else {
          attachToMesh(nullptr);
        }
      }
    });
}

GizmoManager::~GizmoManager()
{
}

void GizmoManager::attachToMesh(AbstractMesh* mesh)
{
  if (_attachedMesh) {
    // _attachedMesh->removeBehavior(boundingBoxDragBehavior);
  }
  _attachedMesh = mesh;

  if (gizmos.positionGizmo && _gizmosEnabled.positionGizmo) {
    gizmos.positionGizmo->attachedMesh = mesh;
  }
  if (gizmos.rotationGizmo && _gizmosEnabled.rotationGizmo) {
    gizmos.rotationGizmo->attachedMesh = mesh;
  }
  if (gizmos.scaleGizmo && _gizmosEnabled.scaleGizmo) {
    gizmos.scaleGizmo->attachedMesh = mesh;
  }
  if (gizmos.boundingBoxGizmo && _gizmosEnabled.boundingBoxGizmo) {
    gizmos.boundingBoxGizmo->attachedMesh = mesh;
  }

  if (boundingBoxGizmoEnabled && _attachedMesh) {
    //  _attachedMesh.addBehavior(boundingBoxDragBehavior);
  }
}

void GizmoManager::set_positionGizmoEnabled(bool value)
{
  if (value) {
    if (!gizmos.positionGizmo) {
      gizmos.positionGizmo
        = std::make_unique<PositionGizmo>(_defaultUtilityLayer);
    }
    gizmos.positionGizmo->attachedMesh = _attachedMesh;
  }
  else if (gizmos.positionGizmo) {
    gizmos.positionGizmo->attachedMesh = nullptr;
  }
  _gizmosEnabled.positionGizmo = value;
}

bool GizmoManager::get_positionGizmoEnabled() const
{
  return _gizmosEnabled.positionGizmo;
}

void GizmoManager::set_rotationGizmoEnabled(bool value)
{
  if (value) {
    if (!gizmos.rotationGizmo) {
      gizmos.rotationGizmo
        = std::make_unique<RotationGizmo>(_defaultUtilityLayer);
    }
    gizmos.rotationGizmo->updateGizmoRotationToMatchAttachedMesh = false;
    gizmos.rotationGizmo->attachedMesh = _attachedMesh;
  }
  else if (gizmos.rotationGizmo) {
    gizmos.rotationGizmo->attachedMesh = nullptr;
  }
  _gizmosEnabled.rotationGizmo = value;
}

bool GizmoManager::get_rotationGizmoEnabled() const
{
  return _gizmosEnabled.rotationGizmo;
}

void GizmoManager::set_scaleGizmoEnabled(bool value)
{
  if (value) {
    if (!gizmos.scaleGizmo) {
      gizmos.scaleGizmo = std::make_unique<ScaleGizmo>(_defaultUtilityLayer);
    }
    gizmos.scaleGizmo->attachedMesh = _attachedMesh;
  }
  else if (gizmos.scaleGizmo) {
    gizmos.scaleGizmo->attachedMesh = nullptr;
  }
  _gizmosEnabled.scaleGizmo = value;
}

bool GizmoManager::get_scaleGizmoEnabled() const
{
  return _gizmosEnabled.scaleGizmo;
}

void GizmoManager::set_boundingBoxGizmoEnabled(bool value)
{
  if (value) {
    if (!gizmos.boundingBoxGizmo) {
      gizmos.boundingBoxGizmo = std::make_unique<BoundingBoxGizmo>(
        _boundingBoxColor, _defaultKeepDepthUtilityLayer);
    }
    gizmos.boundingBoxGizmo->attachedMesh = _attachedMesh;
    if (_attachedMesh) {
      // _attachedMesh->removeBehavior(boundingBoxDragBehavior);
      // _attachedMesh->addBehavior(boundingBoxDragBehavior);
    }
  }
  else if (gizmos.boundingBoxGizmo) {
    gizmos.boundingBoxGizmo->attachedMesh = nullptr;
  }
  _gizmosEnabled.boundingBoxGizmo = value;
}

bool GizmoManager::get_boundingBoxGizmoEnabled() const
{
  return _gizmosEnabled.boundingBoxGizmo;
}

void GizmoManager::dispose(bool /*doNotRecurse*/,
                           bool /*disposeMaterialAndTextures*/)
{
  _scene->onPointerObservable.remove(_pointerObserver);

  if (gizmos.positionGizmo) {
    gizmos.positionGizmo->dispose();
    gizmos.positionGizmo = nullptr;
  }
  if (gizmos.rotationGizmo) {
    gizmos.rotationGizmo->dispose();
    gizmos.rotationGizmo = nullptr;
  }
  if (gizmos.scaleGizmo) {
    gizmos.scaleGizmo->dispose();
    gizmos.scaleGizmo = nullptr;
  }
  if (gizmos.boundingBoxGizmo) {
    gizmos.boundingBoxGizmo->dispose();
    gizmos.boundingBoxGizmo = nullptr;
  }
  _defaultKeepDepthUtilityLayer->dispose();
  _defaultUtilityLayer->dispose();
  boundingBoxDragBehavior->detach();
}

} // end of namespace BABYLON
