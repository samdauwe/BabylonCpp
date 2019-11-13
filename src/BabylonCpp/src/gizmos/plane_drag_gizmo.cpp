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

TransformNodePtr
PlaneDragGizmo::_CreatePlane(Scene* scene, const StandardMaterialPtr& material)
{
  auto plane = TransformNode::New("plane", scene);

  // make sure plane is double sided
  PlaneOptions options;
  options.width           = 0.1375f;
  options.height          = 0.1375f;
  options.sideOrientation = 2u;
  auto dragPlane      = PlaneBuilder::CreatePlane("dragPlane", options, scene);
  dragPlane->material = material;
  dragPlane->parent   = plane.get();

  // Position plane pointing normal to dragPlane normal
  dragPlane->material = material;
  return plane;
}

TransformNodePtr
PlaneDragGizmo::_CreateArrowInstance(Scene* scene,
                                     const TransformNodePtr& arrow)
{
  auto instance = TransformNode::New("arrow", scene);
  for (const auto& mesh : arrow->getChildMeshes()) {
    const auto childInstance
      = std::static_pointer_cast<Mesh>(mesh)->createInstance(mesh->name);
    childInstance->parent = instance.get();
  }
  return instance;
}

PlaneDragGizmo::PlaneDragGizmo(const Vector3& dragPlaneNormal,
                               const Color3& color,
                               const UtilityLayerRendererPtr& iGizmoLayer,
                               PositionGizmo* parent)
    : Gizmo{iGizmoLayer}
    , snapDistance{0.f}
    , isEnabled{this, &PlaneDragGizmo::get_isEnabled,
                &PlaneDragGizmo::set_isEnabled}
    , _pointerObserver{nullptr}
    , _plane{nullptr}
    , _coloredMaterial{nullptr}
    , _hoverMaterial{nullptr}
    , _isEnabled{false}
    , _parent{nullptr}
    , tmpSnapEvent{SnapEvent{0.f}}
{
  _parent = parent;
  // Create Material
  _coloredMaterial
    = StandardMaterial::New("", iGizmoLayer->utilityLayerScene.get());
  _coloredMaterial->diffuseColor  = color;
  _coloredMaterial->specularColor = color.subtract(Color3(0.1f, 0.1f, 0.1f));

  _hoverMaterial
    = StandardMaterial::New("", iGizmoLayer->utilityLayerScene.get());
  _hoverMaterial->diffuseColor = color.add(Color3(0.3f, 0.3f, 0.3f));

  // Build plane mesh on root node
  _plane = PlaneDragGizmo::_CreatePlane(iGizmoLayer->utilityLayerScene.get(),
                                        _coloredMaterial);

  _plane->lookAt(_rootMesh->position().add(dragPlaneNormal));
  _plane->scaling().scaleInPlace(1.f / 3.f);
  _plane->parent = _rootMesh.get();

  currentSnapDragDistance = 0.f;
  // Add dragPlaneNormal drag behavior to handle events when the gizmo is
  // dragged
  PointerDragBehaviorOptions options;
  options.dragPlaneNormal    = dragPlaneNormal;
  dragBehavior               = std::make_shared<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  // _rootMesh->addBehavior(dragBehavior);

  dragBehavior->onDragObservable.add([this](DragMoveEvent* event,
                                            EventState & /*es*/) -> void {
    if (attachedMesh()) {
      currentSnapDragDistance = 0.f;
      Vector3 localDelta;
      Matrix tmpMatrix;
      // Convert delta to local translation if it has a parent
      if (attachedMesh()->parent()) {
        attachedMesh()->parent()->computeWorldMatrix().invertToRef(tmpMatrix);
        tmpMatrix.setTranslationFromFloats(0.f, 0.f, 0.f);
        Vector3::TransformCoordinatesToRef(event->delta, tmpMatrix, localDelta);
      }
      else {
        localDelta.copyFrom(event->delta);
      }
      // Snapping logic
      if (snapDistance == 0.f) {
        attachedMesh()->position().addInPlace(localDelta);
      }
      else {
        currentSnapDragDistance += event->dragDistance;
        if (std::abs(currentSnapDragDistance) > snapDistance) {
          auto dragSteps
            = std::floor(std::abs(currentSnapDragDistance) / snapDistance);
          currentSnapDragDistance
            = std::fmod(currentSnapDragDistance, snapDistance);
          localDelta.normalizeToRef(tmpVector);
          tmpVector.scaleInPlace(snapDistance * dragSteps);
          attachedMesh()->position().addInPlace(tmpVector);
          tmpSnapEvent.snapDistance = snapDistance * dragSteps;
          onSnapObservable.notifyObservers(&tmpSnapEvent);
        }
      }
    }
  });

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [this](PointerInfo* pointerInfo, EventState & /*es*/) -> void {
      if (_customMeshSet) {
        return;
      }
      const auto isHovered = stl_util::contains(
        _rootMesh->getChildMeshes(), pointerInfo->pickInfo.pickedMesh);
      const auto material = isHovered ? _hoverMaterial : _coloredMaterial;
      for (const auto& m : _rootMesh->getChildMeshes()) {
        m->material = material;
      }
    });

  auto light = gizmoLayer->_getSharedGizmoLight();
  stl_util::concat(light->includedOnlyMeshes(),
                   _rootMesh->getChildMeshes(false));
}

PlaneDragGizmo::~PlaneDragGizmo() = default;

void PlaneDragGizmo::_attachedMeshChanged(const AbstractMeshPtr& value)
{
  dragBehavior->enabled = value ? true : false;
}

void PlaneDragGizmo::set_isEnabled(bool value)
{
  _isEnabled = value;
  if (!value) {
    attachedMesh = nullptr;
  }
  else {
    if (_parent) {
      attachedMesh = _parent->attachedMesh();
    }
  }
}

bool PlaneDragGizmo::get_isEnabled() const
{
  return _isEnabled;
}

void PlaneDragGizmo::dispose(bool /*doNotRecurse*/,
                             bool /*disposeMaterialAndTextures*/)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  dragBehavior->detach();
  Gizmo::dispose();
  if (_plane) {
    _plane->dispose();
  }
  for (const auto& matl : {_coloredMaterial, _hoverMaterial}) {
    if (matl) {
      matl->dispose();
    }
  }
}

} // namespace BABYLON
