#include <babylon/gizmos/axis_scale_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/mesh/pointer_drag_behavior.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

AxisScaleGizmo::AxisScaleGizmo(UtilityLayerRenderer* gizmoLayer,
                               const Vector3& dragAxis, const Color3& color)
    : Gizmo{gizmoLayer}
    , snapDistance{0.f}
    , _pointerObserver{nullptr}
    , _currentSnapDragDistance{0.f}
    , _tmpSnapEvent{0.f}
{
  // Create Material
  auto coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  coloredMaterial->setDisableLighting(true);
  coloredMaterial->emissiveColor = color;

  auto hoverMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  hoverMaterial->setDisableLighting(true);
  hoverMaterial->emissiveColor = color.add(Color3(0.2f, 0.2f, 0.2f));

  // Build mesh on root node
  auto arrow = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  BoxOptions boxOptions{0.5f};
  auto arrowMesh = MeshBuilder::CreateBox("yPosMesh", boxOptions,
                                          gizmoLayer->utilityLayerScene.get());
  CylinderOptions cylinderOptions{0.015f};
  cylinderOptions.height       = 0.3f;
  cylinderOptions.tessellation = 96;
  auto arrowTail               = MeshBuilder::CreateCylinder(
    "yPosMesh", cylinderOptions, gizmoLayer->utilityLayerScene.get());
  arrow->addChild(arrowMesh);
  arrow->addChild(arrowTail);

  // Position arrow pointing in its drag axis
  arrowMesh->scaling().scaleInPlace(0.1f);
  arrowMesh->material     = coloredMaterial;
  arrowMesh->rotation().x = Math::PI_2;
  arrowMesh->position().z += 0.3f;
  arrowTail->rotation().x = Math::PI_2;
  arrowTail->material     = coloredMaterial;
  arrowTail->position().z += 0.15f;
  arrow->lookAt(_rootMesh->position().subtract(dragAxis));
  _rootMesh->addChild(arrow);

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragAxis = dragAxis;
  _dragBehavior    = ::std::make_unique<PointerDragBehavior>(options);
  _dragBehavior->moveAttached = false;
  _rootMesh->addBehavior(_dragBehavior.get());

  _dragBehavior->onDragObservable.add(
    [&](DragMoveEvent* event, EventState& /*es*/) {
      if (attachedMesh) {
        // Snapping logic
        auto snapped   = false;
        auto dragSteps = 0;
        if (snapDistance == 0.f) {
          dragAxis.scaleToRef(event->dragDistance, _tmpVector);
        }
        else {
          _currentSnapDragDistance += event->dragDistance;
          if (::std::abs(_currentSnapDragDistance) > snapDistance) {
            dragSteps = static_cast<int>(
              ::std::floor(_currentSnapDragDistance / snapDistance));
            _currentSnapDragDistance
              = ::std::fmod(_currentSnapDragDistance, snapDistance);
            dragAxis.scaleToRef(snapDistance * dragSteps, _tmpVector);
            snapped = true;
          }
          else {
            _tmpVector.scaleInPlace(0.f);
          }
        }

        auto invertCount = 0;
        if (attachedMesh()->scaling().x < 0.f) {
          invertCount++;
        }
        if (attachedMesh()->scaling().y < 0.f) {
          invertCount++;
        }
        if (attachedMesh()->scaling().z < 0.f) {
          invertCount++;
        }
        if (invertCount % 2 == 0) {
          attachedMesh()->scaling().addInPlace(_tmpVector);
        }
        else {
          attachedMesh()->scaling().subtractInPlace(_tmpVector);
        }

        if (snapped) {
          _tmpSnapEvent.snapDistance = snapDistance * dragSteps;
          onSnapObservable.notifyObservers(&_tmpSnapEvent);
        }
      }
    });

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (stl_util::contains(_rootMesh->getChildMeshes(),
                             pointerInfo->pickInfo.pickedMesh)) {
        for (auto& m : _rootMesh->getChildMeshes()) {
          m->material = hoverMaterial;
        }
      }
      else {
        for (auto& m : _rootMesh->getChildMeshes()) {
          m->material = coloredMaterial;
        }
      }
    });
}

AxisScaleGizmo::~AxisScaleGizmo()
{
}

void AxisScaleGizmo::_attachedMeshChanged(AbstractMesh* value)
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

} // end of namespace BABYLON
