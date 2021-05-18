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

TransformNodePtr AxisDragGizmo::_CreateArrow(Scene* scene, const StandardMaterialPtr& material,
                                             float thickness, bool isCollider)
{
  auto arrow = TransformNode::New("arrow", scene);
  CylinderOptions cylinder1Options;
  cylinder1Options.diameterTop    = 0.f;
  cylinder1Options.height         = 0.075f;
  cylinder1Options.diameterBottom = 0.0375f * (1.f + (thickness - 1.f) / 4.f);
  cylinder1Options.tessellation   = 96;
  auto cylinder = CylinderBuilder::CreateCylinder("cylinder", cylinder1Options, scene);
  CylinderOptions cylinder2Options;
  cylinder2Options.diameterTop    = 0.005f * thickness;
  cylinder2Options.height         = 0.275f;
  cylinder2Options.diameterBottom = 0.005f * thickness;
  cylinder2Options.tessellation   = 96;
  auto line      = CylinderBuilder::CreateCylinder("cylinder", cylinder2Options, scene);
  line->material = material;

  // Position arrow pointing in its drag axis
  cylinder->parent       = arrow.get();
  cylinder->material     = material;
  cylinder->rotation().x = Math::PI / 2.f;
  cylinder->position().z += 0.3f;

  line->parent   = arrow.get();
  line->material = material;
  line->position().z += 0.275f / 2.f;
  line->rotation().x = Math::PI / 2.f;

  if (isCollider) {
    line->visibility     = 0.f;
    cylinder->visibility = 0.f;
  }
  return arrow;
}

TransformNodePtr AxisDragGizmo::_CreateArrowInstance(Scene* scene, const TransformNodePtr& arrow)
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
                             const UtilityLayerRendererPtr& iGizmoLayer, PositionGizmo* parent,
                             float thickness)
    : Gizmo{iGizmoLayer}
    , snapDistance{0.f}
    , isEnabled{this, &AxisDragGizmo::get_isEnabled, &AxisDragGizmo::set_isEnabled}
    , _pointerObserver{nullptr}
    , _isEnabled{true}
    , _parent{nullptr}
    , _gizmoMesh{nullptr}
    , _coloredMaterial{nullptr}
    , _hoverMaterial{nullptr}
    , _disableMaterial{nullptr}
    , _dragging{false}
    , _currentSnapDragDistance{0.f}
    , _tmpSnapEvent{0.f}
{
  _parent = parent;

  // Create Material
  _coloredMaterial                = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _coloredMaterial->diffuseColor  = color;
  _coloredMaterial->specularColor = color.subtract(Color3(0.1f, 0.1f, 0.1f));

  _hoverMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _hoverMaterial->diffuseColor = Color3::Yellow();

  _disableMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _disableMaterial->diffuseColor = Color3::Gray();
  _disableMaterial->alpha        = 0.4f;

  // Build Mesh + Collider
  const auto arrow = std::static_pointer_cast<AbstractMesh>(
    AxisDragGizmo::_CreateArrow(gizmoLayer->utilityLayerScene.get(), _coloredMaterial, thickness));
  const auto collider = std::static_pointer_cast<AbstractMesh>(AxisDragGizmo::_CreateArrow(
    gizmoLayer->utilityLayerScene.get(), _coloredMaterial, thickness + 4.f, true));

  // Add to Root Node
  _gizmoMesh = Mesh::New("", gizmoLayer->utilityLayerScene.get());
  _gizmoMesh->addChild(*arrow);
  _gizmoMesh->addChild(*collider);

  _gizmoMesh->lookAt(_rootMesh->position().add(dragAxis));
  _gizmoMesh->scaling().scaleInPlace(1.f / 3.f);
  _gizmoMesh->parent = _rootMesh.get();

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragAxis = dragAxis;
  // options.pointerObservableScene = gizmoLayer->originalScene;
  dragBehavior               = std::make_shared<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  // _rootMesh->addBehavior(dragBehavior.get());

  dragBehavior->onDragObservable.add([&](DragMoveEvent* event, EventState& /*es*/) {
    if (attachedNode()) {
      // Keep world translation and use it to update world transform
      // if the node has parent, the local transform properties (position, rotation, scale)
      // will be recomputed in _matrixChanged function

      // Snapping logic
      if (snapDistance == 0.f) {
        auto transformNode = std::static_pointer_cast<TransformNode>(attachedNode());
        if (transformNode) { // Required for nodes like lights
          transformNode->position().addInPlaceFromFloats(event->delta.x, event->delta.y,
                                                         event->delta.z);
        }
        // use _worldMatrix to not force a matrix update when calling GetWorldMatrix especially with
        // Cameras
        attachedNode()->getWorldMatrix().addTranslationFromFloats(event->delta.x, event->delta.y,
                                                                  event->delta.z);
        attachedNode()->updateCache();
      }
      else {
        _currentSnapDragDistance += event->dragDistance;
        if (std::abs(_currentSnapDragDistance) > snapDistance) {
          auto dragSteps           = std::floor(std::abs(_currentSnapDragDistance) / snapDistance);
          _currentSnapDragDistance = std::fmod(_currentSnapDragDistance, snapDistance);
          event->delta.normalizeToRef(_tmpVector);
          _tmpVector.scaleInPlace(snapDistance * dragSteps);
          attachedNode()->getWorldMatrix().addTranslationFromFloats(_tmpVector.x, _tmpVector.y,
                                                                    _tmpVector.z);
          attachedNode()->updateCache();
          _tmpSnapEvent.snapDistance = snapDistance * dragSteps;
          onSnapObservable.notifyObservers(&_tmpSnapEvent);
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

  _cache.gizmoMeshes     = toMeshArray(arrow->getChildMeshes());
  _cache.colliderMeshes  = toMeshArray(collider->getChildMeshes());
  _cache.material        = _coloredMaterial;
  _cache.hoverMaterial   = _hoverMaterial;
  _cache.disableMaterial = _disableMaterial;
  _cache.active          = false;
  _cache.dragBehavior    = dragBehavior;
  if (_parent) {
    _parent->addToAxisCache(static_cast<Mesh*>(collider.get()), _cache);
  }

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (_customMeshSet) {
        return;
      }

      auto pickedMesh = std::static_pointer_cast<Mesh>(pointerInfo->pickInfo.pickedMesh);
      _isHovered      = stl_util::contains(_cache.colliderMeshes, pickedMesh);

      if (!_parent) {
        const auto material = dragBehavior && dragBehavior->enabled ?
                                (_isHovered || _dragging ? _hoverMaterial : _coloredMaterial) :
                                _disableMaterial;
        for (const auto& m : _cache.gizmoMeshes) {
          m->material          = material;
          const auto linesMesh = std::static_pointer_cast<LinesMesh>(m);
          if (linesMesh && dragBehavior->enabled) {
            linesMesh->color = material->diffuseColor;
          }
        }
      }
    });
}

AxisDragGizmo::~AxisDragGizmo() = default;

void AxisDragGizmo::_attachedNodeChanged(const NodePtr& value)
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
    attachedNode = nullptr;
  }
  else {
    if (_parent) {
      attachedMesh = _parent->attachedMesh();
      attachedNode = _parent->attachedNode();
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
  if (_gizmoMesh) {
    _gizmoMesh->dispose();
  }
  for (const auto& matl : {_coloredMaterial, _hoverMaterial, _disableMaterial}) {
    if (matl) {
      matl->dispose();
    }
  }
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
