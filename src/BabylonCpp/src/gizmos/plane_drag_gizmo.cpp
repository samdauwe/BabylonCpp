#include <babylon/gizmos/plane_drag_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>
#include <babylon/gizmos/position_gizmo.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/plane_builder.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/transform_node.h>

namespace BABYLON {

TransformNodePtr PlaneDragGizmo::_CreatePlane(Scene* scene, const StandardMaterialPtr& material)
{
  auto plane = TransformNode::New("plane", scene);

  // make sure plane is double sided
  PlaneOptions options;
  options.width           = 0.1375f;
  options.height          = 0.1375f;
  options.sideOrientation = 2u;
  auto dragPlane          = PlaneBuilder::CreatePlane("dragPlane", options, scene);
  dragPlane->material     = material;
  dragPlane->parent       = plane.get();

  return plane;
}

PlaneDragGizmo::PlaneDragGizmo(const Vector3& dragPlaneNormal, const Color3& color,
                               const UtilityLayerRendererPtr& iGizmoLayer, PositionGizmo* parent)
    : Gizmo{iGizmoLayer}
    , snapDistance{0.f}
    , isEnabled{this, &PlaneDragGizmo::get_isEnabled, &PlaneDragGizmo::set_isEnabled}
    , _pointerObserver{nullptr}
    , _gizmoMesh{nullptr}
    , _coloredMaterial{nullptr}
    , _hoverMaterial{nullptr}
    , _disableMaterial{nullptr}
    , _isEnabled{false}
    , _parent{nullptr}
    , _dragging{false}
    , tmpSnapEvent{SnapEvent{0.f}}
{
  _parent = parent;
  // Create Material
  _coloredMaterial                = StandardMaterial::New("", iGizmoLayer->utilityLayerScene.get());
  _coloredMaterial->diffuseColor  = color;
  _coloredMaterial->specularColor = color.subtract(Color3(0.1f, 0.1f, 0.1f));

  _hoverMaterial               = StandardMaterial::New("", iGizmoLayer->utilityLayerScene.get());
  _hoverMaterial->diffuseColor = Color3::Yellow();

  _disableMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _disableMaterial->diffuseColor = Color3::Gray();
  _disableMaterial->alpha        = 0.4f;

  // Build plane mesh on root node
  _gizmoMesh = PlaneDragGizmo::_CreatePlane(gizmoLayer->utilityLayerScene.get(), _coloredMaterial);

  _gizmoMesh->lookAt(_rootMesh->position().add(dragPlaneNormal));
  _gizmoMesh->scaling().scaleInPlace(1.f / 3.f);
  _gizmoMesh->parent = _rootMesh.get();

  currentSnapDragDistance = 0.f;
  // Add dragPlaneNormal drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragPlaneNormal    = dragPlaneNormal;
  dragBehavior               = std::make_shared<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  // _rootMesh->addBehavior(dragBehavior);

  dragBehavior->onDragObservable.add([this](DragMoveEvent* event, EventState& /*es*/) -> void {
    if (attachedNode()) {
      // Keep world translation and use it to update world transform
      // if the node has parent, the local transform properties (position, rotation, scale)
      // will be recomputed in _matrixChanged function

      // Snapping logic
      if (snapDistance == 0.f) {
        attachedNode()->getWorldMatrix().addTranslationFromFloats(event->delta.x, event->delta.y,
                                                                  event->delta.z);
      }
      else {
        currentSnapDragDistance += event->dragDistance;
        if (std::abs(currentSnapDragDistance) > snapDistance) {
          auto dragSteps          = std::floor(std::abs(currentSnapDragDistance) / snapDistance);
          currentSnapDragDistance = std::fmod(currentSnapDragDistance, snapDistance);
          event->delta.normalizeToRef(tmpVector);
          tmpVector.scaleInPlace(snapDistance * dragSteps);
          attachedNode()->getWorldMatrix().addTranslationFromFloats(tmpVector.x, tmpVector.y,
                                                                    tmpVector.z);
          tmpSnapEvent.snapDistance = snapDistance * dragSteps;
          onSnapObservable.notifyObservers(&tmpSnapEvent);
        }
      }
      _matrixChanged();
    }
  });
  dragBehavior->onDragStartObservable.add(
    [this](DragStartOrEndEvent* /*evt*/, EventState& /*es*/) -> void { _dragging = true; });
  dragBehavior->onDragEndObservable.add(
    [this](DragStartOrEndEvent* /*evt*/, EventState& /*es*/) -> void { _dragging = false; });

  auto light = gizmoLayer->_getSharedGizmoLight();
  light->includedOnlyMeshes
    = stl_util::concat(light->includedOnlyMeshes(), _rootMesh->getChildMeshes(false));

  const auto toMeshArray = [](const std::vector<AbstractMeshPtr>& meshes) -> std::vector<MeshPtr> {
    std::vector<MeshPtr> meshArray;
    meshArray.reserve(meshes.size());
    for (const auto& mesh : meshes) {
      meshArray.emplace_back(std::static_pointer_cast<Mesh>(mesh));
    }
    return meshArray;
  };

  _cache.gizmoMeshes     = toMeshArray(_gizmoMesh->getChildMeshes());
  _cache.colliderMeshes  = toMeshArray(_gizmoMesh->getChildMeshes());
  _cache.material        = _coloredMaterial;
  _cache.hoverMaterial   = _hoverMaterial;
  _cache.disableMaterial = _disableMaterial;
  _cache.active          = false;
  if (_parent) {
    _parent->addToAxisCache(static_cast<Mesh*>(_gizmoMesh.get()), _cache);
  }

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [this](PointerInfo* pointerInfo, EventState& /*es*/) -> void {
      if (_customMeshSet) {
        return;
      }

      auto pickedMesh = std::static_pointer_cast<Mesh>(pointerInfo->pickInfo.pickedMesh);
      _isHovered      = stl_util::contains(_cache.colliderMeshes, pickedMesh);

      if (!_parent) {
        const auto material = _isHovered || _dragging ? _hoverMaterial : _coloredMaterial;
        for (const auto& m : _cache.gizmoMeshes) {
          m->material = material;
        }
      }
    });
}

PlaneDragGizmo::~PlaneDragGizmo() = default;

void PlaneDragGizmo::_attachedNodeChanged(const NodePtr& value)
{
  dragBehavior->enabled = value ? true : false;
}

void PlaneDragGizmo::set_isEnabled(bool value)
{
  _isEnabled = value;
  if (!value) {
    attachedNode = nullptr;
  }
  else {
    if (_parent) {
      attachedNode = _parent->attachedNode();
    }
  }
}

bool PlaneDragGizmo::get_isEnabled() const
{
  return _isEnabled;
}

void PlaneDragGizmo::dispose(bool /*doNotRecurse*/, bool /*disposeMaterialAndTextures*/)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  dragBehavior->detach();
  Gizmo::dispose();
  if (_gizmoMesh) {
    _gizmoMesh->dispose();
  }
  for (const auto& matl : {_coloredMaterial, _hoverMaterial, _disableMaterial}) {
    if (matl) {
      matl->dispose();
    }
  }
}

} // namespace BABYLON
