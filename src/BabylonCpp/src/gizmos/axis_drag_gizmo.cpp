#include <babylon/gizmos/axis_drag_gizmo.h>

#include <babylon/behaviors/mesh/pointer_drag_behavior.h>

#include <babylon/materials/standard_material.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

AxisDragGizmo::AxisDragGizmo(UtilityLayerRenderer* gizmoLayer,
                             const Vector3& dragAxis, const Color3& color)
    : Gizmo{gizmoLayer}
{
  // Create Material
  auto coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  coloredMaterial->setDisableLighting(true);
  coloredMaterial->setEmissiveColor(color);

  // Build mesh on root node
  CylinderOptions arrowMeshOptions;
  arrowMeshOptions.diameterTop  = 0.f;
  arrowMeshOptions.height       = 2.f;
  arrowMeshOptions.tessellation = 96;
  auto arrowMesh                = MeshBuilder::CreateCylinder(
    "yPosMesh", arrowMeshOptions, gizmoLayer->utilityLayerScene.get());
  CylinderOptions arrowTailOptions{0.03f};
  arrowTailOptions.height       = 0.2f;
  arrowTailOptions.tessellation = 96;
  auto arrowTail                = MeshBuilder::CreateCylinder(
    "yPosMesh", arrowTailOptions, gizmoLayer->utilityLayerScene.get());
  _rootMesh->addChild(arrowMesh);
  _rootMesh->addChild(arrowTail);

  // Position arrow pointing in its drag axis
  arrowMesh->scaling().scaleInPlace(0.1f);
  arrowMesh->setMaterial(coloredMaterial);
  arrowMesh->rotation().x = Math::PI_2;
  arrowMesh->position().z += 0.3f;
  arrowTail->rotation().x = Math::PI_2;
  arrowTail->setMaterial(coloredMaterial);
  arrowTail->position().z += 0.2f;
  _rootMesh->lookAt(_rootMesh->position().subtract(dragAxis));

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragAxis               = dragAxis;
  options.pointerObservableScene = gizmoLayer->originalScene;
  _dragBehavior = ::std::make_unique<PointerDragBehavior>(options);
  _dragBehavior->moveAttached = false;
  _rootMesh->addBehavior(_dragBehavior.get());
  _dragBehavior->onDragObservable.add(
    [&](DragMoveEvent* event, EventState& /*es*/) {
      if (attachedMesh) {
        attachedMesh->position().addInPlace(event->delta);
      }
    });
}

AxisDragGizmo::~AxisDragGizmo()
{
}

void AxisDragGizmo::dispose(bool /*doNotRecurse*/,
                            bool /*disposeMaterialAndTextures*/)
{
  _dragBehavior->detach();
  Gizmo::dispose();
}

} // end of namespace BABYLON
