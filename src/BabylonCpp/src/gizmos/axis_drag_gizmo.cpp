#include <babylon/gizmos/axis_drag_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/engines/scene.h>
#include <babylon/gizmos/position_gizmo.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/instanced_mesh.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

TransformNodePtr
AxisDragGizmo::_CreateArrow(Scene* scene, const StandardMaterialPtr& material)
{
  auto arrow = TransformNode::New("arrow", scene);
  CylinderOptions cylinder1Options;
  cylinder1Options.diameterTop    = 0.f;
  cylinder1Options.height         = 0.075f;
  cylinder1Options.diameterBottom = 0.0375f;
  cylinder1Options.tessellation   = 96;
  auto cylinder
    = CylinderBuilder::CreateCylinder("cylinder", cylinder1Options, scene);
  CylinderOptions cylinder2Options;
  cylinder2Options.diameterTop    = 0.005f;
  cylinder2Options.height         = 0.275f;
  cylinder2Options.diameterBottom = 0.005f;
  cylinder2Options.tessellation   = 96;
  auto line
    = CylinderBuilder::CreateCylinder("cylinder", cylinder2Options, scene);
  line->material   = material;
  cylinder->parent = arrow.get();
  line->parent     = arrow.get();

  // Position arrow pointing in its drag axis
  cylinder->material     = material;
  cylinder->rotation().x = Math::PI / 2.f;
  cylinder->position().z += 0.3f;
  line->position().z += 0.275f / 2.f;
  line->rotation().x = Math::PI / 2.f;
  return arrow;
}

TransformNodePtr
AxisDragGizmo::_CreateArrowInstance(Scene* scene, const TransformNodePtr& arrow)
{
  auto instance = TransformNode::New("arrow", scene);
  for (const auto& mesh : arrow->getChildMeshes()) {
    auto childMesh = std::static_pointer_cast<Mesh>(mesh);
    if (childMesh) {
      auto childInstance    = childMesh->createInstance(mesh->name);
      childInstance->parent = instance.get();
    }
  }
  return instance;
}

AxisDragGizmo::AxisDragGizmo(const Vector3& dragAxis, const Color3& color,
                             const UtilityLayerRendererPtr& iGizmoLayer,
                             PositionGizmo* parent)
    : Gizmo{iGizmoLayer}
    , snapDistance{0.f}
    , isEnabled{this, &AxisDragGizmo::get_isEnabled,
                &AxisDragGizmo::set_isEnabled}
    , _pointerObserver{nullptr}
    , _isEnabled{true}
    , _parent{nullptr}
    , _arrow{nullptr}
    , _coloredMaterial{nullptr}
    , _hoverMaterial{nullptr}
    , _currentSnapDragDistance{0.f}
    , _tmpSnapEvent{0.f}
{
  _parent = parent;
  // Create Material
  _coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _coloredMaterial->diffuseColor  = color;
  _coloredMaterial->specularColor = color.subtract(Color3(0.1f, 0.1f, 0.1f));

  _hoverMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _hoverMaterial->diffuseColor = color.add(Color3(0.3f, 0.3f, 0.3f));

  // Build mesh on root node
  _arrow = AxisDragGizmo::_CreateArrow(gizmoLayer->utilityLayerScene.get(),
                                       _coloredMaterial);

  _arrow->lookAt(_rootMesh->position().add(dragAxis));
  _arrow->scaling().scaleInPlace(1.f / 3.f);
  _arrow->parent = _rootMesh.get();

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragAxis = dragAxis;
  // options.pointerObservableScene = gizmoLayer->originalScene;
  dragBehavior               = std::make_unique<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  // _rootMesh->addBehavior(dragBehavior.get());
  dragBehavior->onDragObservable.add([&](DragMoveEvent* event,
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

      auto pickedMesh = pointerInfo->pickInfo.pickedMesh;
      auto it         = std::find(_rootMesh->getChildMeshes().begin(),
                          _rootMesh->getChildMeshes().end(), pickedMesh);
      auto isHovered  = (it != _rootMesh->getChildMeshes().end());

      auto material = isHovered ? _hoverMaterial : _coloredMaterial;
      for (auto& m : _rootMesh->getChildMeshes()) {
        m->material    = material;
        auto linesMesh = std::static_pointer_cast<LinesMesh>(m);
        if (linesMesh) {
          linesMesh->color = material->diffuseColor;
        }
      }
    });

  const auto& light         = gizmoLayer->_getSharedGizmoLight();
  light->includedOnlyMeshes = stl_util::concat(light->includedOnlyMeshes(),
                                               _rootMesh->getChildMeshes());
}

AxisDragGizmo::~AxisDragGizmo()
{
}

void AxisDragGizmo::_attachedMeshChanged(const AbstractMeshPtr& value)
{
  if (dragBehavior) {
    dragBehavior->enabled = value ? true : false;
  }
}

void AxisDragGizmo::set_isEnabled(bool value)
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

bool AxisDragGizmo::get_isEnabled() const
{
  return _isEnabled;
}

void AxisDragGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  dragBehavior->detach();
  if (_arrow) {
    _arrow->dispose();
  }
  for (const auto& matl : {_coloredMaterial, _hoverMaterial}) {
    if (matl) {
      matl->dispose();
    }
  }
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
