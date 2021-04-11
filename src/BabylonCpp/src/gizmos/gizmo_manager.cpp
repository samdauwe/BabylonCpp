#include <babylon/gizmos/gizmo_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/meshes/six_dof_drag_behavior.h>
#include <babylon/engines/scene.h>
#include <babylon/gizmos/bounding_box_gizmo.h>
#include <babylon/gizmos/position_gizmo.h>
#include <babylon/gizmos/rotation_gizmo.h>
#include <babylon/gizmos/scale_gizmo.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

GizmoManager::GizmoManager(Scene* iScene, float thickness,
                           const UtilityLayerRendererPtr& utilityLayer,
                           const UtilityLayerRendererPtr& keepDepthUtilityLayer)
    : clearGizmoOnEmptyPointerEvent{false}
    , boundingBoxDragBehavior{std::make_unique<SixDofDragBehavior>()}
    , attachableMeshes{std::nullopt}
    , attachableNodes{std::nullopt}
    , usePointerToAttachGizmos{true}
    , keepDepthUtilityLayer{this, &GizmoManager::get_keepDepthUtilityLayer}
    , utilityLayer{this, &GizmoManager::get_utilityLayer}
    , isHovered{this, &GizmoManager::get_isHovered}
    , scaleRatio{this, &GizmoManager::get_scaleRatio, &GizmoManager::set_scaleRatio}
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
    , _attachedMesh{nullptr}
    , _attachedNode{nullptr}
    , _boundingBoxColor{Color3::FromHexString("#0984e3")}
    , _thickness{1.f}
    , _scaleRatio{1.f}
{
  _defaultUtilityLayer = utilityLayer ? utilityLayer : UtilityLayerRenderer::DefaultUtilityLayer();
  _defaultKeepDepthUtilityLayer = keepDepthUtilityLayer ?
                                    keepDepthUtilityLayer :
                                    UtilityLayerRenderer::DefaultKeepDepthUtilityLayer();
  _defaultKeepDepthUtilityLayer->utilityLayerScene->autoClearDepthAndStencil = false;
  _defaultUtilityLayer = UtilityLayerRenderer::DefaultUtilityLayer();

  _thickness = thickness;
  gizmos     = {};

  const auto attachToMeshPointerObserver = _attachToMeshPointerObserver(iScene);
  const auto gizmoAxisPointerObserver
    = Gizmo::GizmoAxisPointerObserver(_defaultUtilityLayer, _gizmoAxisCache);
  _pointerObservers = {attachToMeshPointerObserver, gizmoAxisPointerObserver};
}

Observer<PointerInfo>::Ptr GizmoManager::_attachToMeshPointerObserver(Scene* /*scene*/)
{
  // Instantiate/dispose gizmos based on pointer actions
  const auto pointerObserver
    = _scene->onPointerObservable.add([this](PointerInfo* pointerInfo, EventState& /*es*/) {
        if (!usePointerToAttachGizmos) {
          return;
        }
        if (pointerInfo->type == PointerEventTypes::POINTERDOWN) {
          if (pointerInfo->pickInfo.pickedMesh) {
            auto node
              = pointerInfo->pickInfo.pickedMesh ? pointerInfo->pickInfo.pickedMesh : nullptr;
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
                if (node && (node == mesh || node->isDescendantOf(mesh.get()))) {
                  node  = mesh;
                  found = true;
                }
              }
              if (!found) {
                node = nullptr;
              }
            }
            if (node && node->type() == Type::ABSTRACTMESH) {
              if (_attachedMesh != node) {
                attachToMesh(std::static_pointer_cast<AbstractMesh>(node->shared_from_this()));
              }
            }
            else {
              if (clearGizmoOnEmptyPointerEvent) {
                attachToMesh(nullptr);
              }
            }
          }
          else {
            if (clearGizmoOnEmptyPointerEvent) {
              attachToMesh(nullptr);
            }
          }
        }
      });
  return pointerObserver;
}

GizmoManager::~GizmoManager() = default;

void GizmoManager::attachToMesh(const AbstractMeshPtr& mesh)
{
  if (_attachedMesh) {
    // _attachedMesh->removeBehavior(boundingBoxDragBehavior);
  }
  if (_attachedNode) {
    // _attachedNode->removeBehavior(boundingBoxDragBehavior);
  }
  _attachedMesh = mesh;
  _attachedNode = nullptr;
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
  onAttachedToMeshObservable.notifyObservers(mesh.get());
}

void GizmoManager::attachToNode(const NodePtr& node)
{
  if (_attachedMesh) {
    // _attachedMesh->removeBehavior(boundingBoxDragBehavior);
  }
  if (_attachedNode) {
    // _attachedNode->removeBehavior(boundingBoxDragBehavior);
  }
  _attachedMesh = nullptr;
  _attachedNode = node;
  {
    if (gizmos.positionGizmo && _gizmosEnabled.positionGizmo) {
      gizmos.positionGizmo->attachedNode = node;
    }
    if (gizmos.rotationGizmo && _gizmosEnabled.rotationGizmo) {
      gizmos.rotationGizmo->attachedNode = node;
    }
    if (gizmos.scaleGizmo && _gizmosEnabled.scaleGizmo) {
      gizmos.scaleGizmo->attachedNode = node;
    }
    if (gizmos.boundingBoxGizmo && _gizmosEnabled.boundingBoxGizmo) {
      gizmos.boundingBoxGizmo->attachedNode = node;
    }
  }
  if (boundingBoxGizmoEnabled && _attachedNode) {
    // _attachedNode->addBehavior(boundingBoxDragBehavior);
  }
  onAttachedToNodeObservable.notifyObservers(node.get());
}

UtilityLayerRendererPtr& GizmoManager::get_keepDepthUtilityLayer()
{
  return _defaultKeepDepthUtilityLayer;
}

UtilityLayerRendererPtr& GizmoManager::get_utilityLayer()
{
  return _defaultUtilityLayer;
}

bool GizmoManager::get_isHovered() const
{
  bool hovered = false;
  {
    if (gizmos.positionGizmo && gizmos.positionGizmo->isHovered()) {
      hovered = true;
    }
    if (gizmos.rotationGizmo && gizmos.rotationGizmo->isHovered()) {
      hovered = true;
    }
    if (gizmos.scaleGizmo && gizmos.scaleGizmo->isHovered()) {
      hovered = true;
    }
    if (gizmos.boundingBoxGizmo && gizmos.boundingBoxGizmo->isHovered()) {
      hovered = true;
    }
  }
  return hovered;
}

void GizmoManager::set_scaleRatio(float value)
{
  _scaleRatio = value;
  if (gizmos.positionGizmo) {
    gizmos.positionGizmo->scaleRatio = value;
  }
  if (gizmos.rotationGizmo) {
    gizmos.rotationGizmo->scaleRatio = value;
  }
  if (gizmos.scaleGizmo) {
    gizmos.scaleGizmo->scaleRatio = value;
  }
}

float GizmoManager::get_scaleRatio() const
{
  return _scaleRatio;
}

void GizmoManager::set_positionGizmoEnabled(bool value)
{
  if (value) {
    if (!gizmos.positionGizmo) {
      gizmos.positionGizmo
        = std::make_unique<PositionGizmo>(_defaultUtilityLayer, _thickness, this);
    }
    if (_attachedNode) {
      gizmos.positionGizmo->attachedNode = _attachedNode;
    }
    else {
      gizmos.positionGizmo->attachedMesh = _attachedMesh;
    }
  }
  else if (gizmos.positionGizmo) {
    gizmos.positionGizmo->attachedNode = nullptr;
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
        = std::make_unique<RotationGizmo>(_defaultUtilityLayer, 32, false, _thickness, this);
    }
    if (_attachedNode) {
      gizmos.rotationGizmo->attachedNode = _attachedNode;
    }
    else {
      gizmos.rotationGizmo->attachedMesh = _attachedMesh;
    }
  }
  else if (gizmos.rotationGizmo) {
    gizmos.rotationGizmo->attachedNode = nullptr;
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
      gizmos.scaleGizmo = std::make_unique<ScaleGizmo>(_defaultUtilityLayer, _thickness, this);
    }
    if (_attachedNode) {
      gizmos.scaleGizmo->attachedNode = _attachedNode;
    }
    else {
      gizmos.scaleGizmo->attachedMesh = _attachedMesh;
    }
  }
  else if (gizmos.scaleGizmo) {
    gizmos.scaleGizmo->attachedNode = nullptr;
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
      gizmos.boundingBoxGizmo
        = std::make_unique<BoundingBoxGizmo>(_boundingBoxColor, _defaultKeepDepthUtilityLayer);
    }
    if (_attachedMesh) {
      gizmos.boundingBoxGizmo->attachedMesh = _attachedMesh;
    }
    else {
      gizmos.boundingBoxGizmo->attachedNode = _attachedNode;
    }

    if (_attachedMesh) {
      // _attachedMesh->removeBehavior(boundingBoxDragBehavior);
      // _attachedMesh->addBehavior(boundingBoxDragBehavior);
    }
    else if (_attachedNode) {
      // _attachedNode->removeBehavior(boundingBoxDragBehavior);
      // _attachedNode->addBehavior(boundingBoxDragBehavior);
    }
  }
  else if (gizmos.boundingBoxGizmo) {
    if (_attachedMesh) {
      // _attachedMesh->removeBehavior(boundingBoxDragBehavior);
    }
    else if (_attachedNode) {
      // _attachedNode->removeBehavior(boundingBoxDragBehavior);
    }
    gizmos.boundingBoxGizmo->attachedNode = nullptr;
  }
  _gizmosEnabled.boundingBoxGizmo = value;
}

bool GizmoManager::get_boundingBoxGizmoEnabled() const
{
  return _gizmosEnabled.boundingBoxGizmo;
}

void GizmoManager::addToAxisCache(const std::unordered_map<Mesh*, GizmoAxisCache>& gizmoAxisCache)
{
  if (!gizmoAxisCache.empty()) {
    for (const auto& [k, v] : gizmoAxisCache) {
      _gizmoAxisCache[k] = v;
    }
  }
}

void GizmoManager::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  for (const auto& observer : _pointerObservers) {
    _scene->onPointerObservable.remove(observer);
  }
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
  onAttachedToMeshObservable.clear();
}

} // end of namespace BABYLON
