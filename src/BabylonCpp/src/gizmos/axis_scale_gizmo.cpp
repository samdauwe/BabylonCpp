#include <babylon/gizmos/axis_scale_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/mesh/pointer_drag_behavior.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

AxisScaleGizmo::AxisScaleGizmo(
  const Vector3& dragAxis, const Color3& color,
  const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , snapDistance{0.f}
    , uniformScaling{false}
    , _pointerObserver{nullptr}
    , _currentSnapDragDistance{0.f}
    , _tmpSnapEvent{0.f}
{
  // Create Material
  _coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _coloredMaterial->disableLighting = true;
  _coloredMaterial->emissiveColor   = color;

  auto hoverMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  hoverMaterial->disableLighting = true;
  hoverMaterial->emissiveColor   = color.add(Color3(0.3f, 0.3f, 0.3f));

  // Build mesh on root node
  auto arrow = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  BoxOptions boxOptions{0.4f};
  auto arrowMesh = MeshBuilder::CreateBox("yPosMesh", boxOptions,
                                          gizmoLayer->utilityLayerScene.get());

  LinesOptions arrowTailOptions;
  arrowTailOptions.points = {Vector3(0.f, 0.f, 0.f), Vector3(0.f, 1.1f, 0.f)};
  auto arrowTail          = MeshBuilder::CreateLines(
    "yPosMesh", arrowTailOptions, gizmoLayer->utilityLayerScene.get());
  arrowTail->color = _coloredMaterial->emissiveColor;
  arrow->addChild(*arrowMesh);
  arrow->addChild(*arrowTail);

  // Position arrow pointing in its drag axis
  arrowMesh->scaling().scaleInPlace(0.1f);
  arrowMesh->material     = _coloredMaterial;
  arrowMesh->rotation().x = Math::PI_2;
  arrowMesh->position().z += 0.3f;
  arrowTail->scaling().scaleInPlace(0.26f);
  arrowTail->rotation().x = Math::PI_2;
  arrowTail->material     = _coloredMaterial;
  arrow->lookAt(_rootMesh->position().subtract(dragAxis));
  _rootMesh->addChild(*arrow);
  arrow->scaling().scaleInPlace(1.f / 3.f);

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragAxis            = dragAxis;
  _dragBehavior               = std::make_unique<PointerDragBehavior>(options);
  _dragBehavior->moveAttached = false;
  _rootMesh->addBehavior(_dragBehavior.get());

  _dragBehavior->onDragObservable.add(
    [&](DragMoveEvent* event, EventState& /*es*/) {
      if (attachedMesh()) {
        // Snapping logic
        auto snapped   = false;
        auto dragSteps = 0;
        if (uniformScaling) {
          attachedMesh()->scaling().normalizeToRef(_tmpVector);
          if (_tmpVector.y < 0.f) {
            _tmpVector.scaleInPlace(-1.f);
          }
        }
        else {
          _tmpVector.copyFrom(dragAxis);
        }
        if (snapDistance == 0.f) {
          _tmpVector.scaleToRef(event->dragDistance, _tmpVector);
        }
        else {
          _currentSnapDragDistance += event->dragDistance;
          if (std::abs(_currentSnapDragDistance) > snapDistance) {
            dragSteps = static_cast<int>(
              std::floor(_currentSnapDragDistance / snapDistance));
            _currentSnapDragDistance
              = std::fmod(_currentSnapDragDistance, snapDistance);
            _tmpVector.scaleToRef(snapDistance * dragSteps, _tmpVector);
            snapped = true;
          }
          else {
            _tmpVector.scaleInPlace(0.f);
          }
        }

        attachedMesh()->scaling().addInPlace(_tmpVector);

        if (snapped) {
          _tmpSnapEvent.snapDistance = snapDistance * dragSteps;
          onSnapObservable.notifyObservers(&_tmpSnapEvent);
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
      auto material = isHovered ? hoverMaterial : _coloredMaterial;
      for (auto& m : _rootMesh->getChildMeshes()) {
        m->material    = material;
        auto linesMesh = std::static_pointer_cast<LinesMesh>(m);
        if (linesMesh) {
          linesMesh->color = material->emissiveColor;
        }
      };
    });
}

AxisScaleGizmo::~AxisScaleGizmo()
{
}

void AxisScaleGizmo::_attachedMeshChanged(const AbstractMeshPtr& value)
{
  if (_dragBehavior) {
    _dragBehavior->enabled = value ? true : false;
  }
}

void AxisScaleGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  _dragBehavior->detach();
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

void AxisScaleGizmo::setCustomMesh(const MeshPtr& mesh, bool useGizmoMaterial)
{
  Gizmo::setCustomMesh(mesh);
  if (useGizmoMaterial) {
    for (const auto& m : _rootMesh->getChildMeshes()) {
      m->material = _coloredMaterial;
      if (auto lm = std::static_pointer_cast<LinesMesh>(m)) {
        lm->color = _coloredMaterial->emissiveColor;
      }
    }
    _customMeshSet = false;
  }
}

} // end of namespace BABYLON
