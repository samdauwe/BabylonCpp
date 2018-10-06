#include <babylon/gizmos/axis_drag_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/mesh/pointer_drag_behavior.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

AxisDragGizmo::AxisDragGizmo(
  const Vector3& dragAxis, const Color3& color,
  const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , snapDistance{0.f}
    , _pointerObserver{nullptr}
    , _currentSnapDragDistance{0.f}
    , _tmpSnapEvent{0.f}
{
  // Create Material
  auto coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  coloredMaterial->disableLighting = true;
  coloredMaterial->emissiveColor   = color;

  auto hoverMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  hoverMaterial->disableLighting = true;
  hoverMaterial->emissiveColor   = color.add(Color3(0.3f, 0.3f, 0.3f));

  // Build mesh on root node
  auto arrow = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  CylinderOptions arrowMeshOptions;
  arrowMeshOptions.diameterTop  = 0.75f;
  arrowMeshOptions.height       = 1.5f;
  arrowMeshOptions.tessellation = 96;
  auto arrowMesh                = MeshBuilder::CreateCylinder(
    "yPosMesh", arrowMeshOptions, gizmoLayer->utilityLayerScene.get());

  LinesOptions arrowTailOptions;
  arrowTailOptions.points = {Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.1f, 0.f)};
  auto arrowTail          = MeshBuilder::CreateLines(
    "yPosMesh", arrowTailOptions, gizmoLayer->utilityLayerScene.get());
  arrowTail->color = coloredMaterial->emissiveColor;
  arrow->addChild(*arrowMesh);
  arrow->addChild(*arrowTail);

  // Position arrow pointing in its drag axis
  arrowMesh->scaling().scaleInPlace(0.05f);
  arrowMesh->material     = coloredMaterial;
  arrowMesh->rotation().x = Math::PI_2;
  arrowMesh->position().z += 0.3f;
  arrowTail->scaling().scaleInPlace(0.26f);
  arrowTail->rotation().x = Math::PI_2;
  arrowTail->material     = coloredMaterial;
  arrow->lookAt(_rootMesh->position().subtract(dragAxis));
  arrow->scaling().scaleInPlace(1.f / 3.f);

  _rootMesh->addChild(*arrow);

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragAxis = dragAxis;
  // options.pointerObservableScene = gizmoLayer->originalScene;
  _dragBehavior               = std::make_unique<PointerDragBehavior>(options);
  _dragBehavior->moveAttached = false;
  _rootMesh->addBehavior(_dragBehavior.get());
  _dragBehavior->onDragObservable.add([&](DragMoveEvent* event,
                                          EventState& /*es*/) {
    if (attachedMesh()) {
      // Convert delta to local translation if it has a parent
      if (attachedMesh()->parent()) {
        attachedMesh()->parent()->computeWorldMatrix().invertToRef(_tmpMatrix);
        _tmpMatrix.setTranslationFromFloats(0.f, 0.f, 0.f);
        Vector3::TransformCoordinatesToRef(event->delta, _tmpMatrix,
                                           _localDelta);
      }
      else {
        _localDelta.copyFrom(event->delta);
      }
      // Snapping logic
      if (snapDistance == 0.f) {
        attachedMesh()->position().addInPlace(_localDelta);
      }
      else {
        _currentSnapDragDistance += event->dragDistance;
        if (std::abs(_currentSnapDragDistance) > snapDistance) {
          auto dragSteps
            = std::floor(std::abs(_currentSnapDragDistance) / snapDistance);
          _currentSnapDragDistance
            = std::fmod(_currentSnapDragDistance, snapDistance);
          _localDelta.normalizeToRef(_tmpVector);
          _tmpVector.scaleInPlace(snapDistance * dragSteps);
          attachedMesh()->position().addInPlace(_tmpVector);
          _tmpSnapEvent.snapDistance = snapDistance * dragSteps;
          onSnapObservable.notifyObservers(&_tmpSnapEvent);
        }
      }
    }
  });

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (_customMeshSet) {
        return;
      }
      auto isHovered = stl_util::contains(
        _rootMesh->getChildMeshes(),
        std::static_pointer_cast<Mesh>(pointerInfo->pickInfo.pickedMesh));
      auto material = isHovered ? hoverMaterial : coloredMaterial;
      for (auto& m : _rootMesh->getChildMeshes()) {
        m->material    = material;
        auto linesMesh = std::static_pointer_cast<LinesMesh>(m);
        if (linesMesh) {
          linesMesh->color = material->emissiveColor;
        }
      };
    });
}

AxisDragGizmo::~AxisDragGizmo()
{
}

void AxisDragGizmo::_attachedMeshChanged(const AbstractMeshPtr& value)
{
  if (_dragBehavior) {
    _dragBehavior->enabled = value ? true : false;
  }
}

void AxisDragGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  _dragBehavior->detach();
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
