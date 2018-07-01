#include <babylon/gizmos/bounding_box_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/mesh/pointer_drag_behavior.h>
#include <babylon/core/string.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

BoundingBoxGizmo::BoundingBoxGizmo(UtilityLayerRenderer* gizmoLayer,
                                   const Color3& color)
    : Gizmo{gizmoLayer}
    , _lineBoundingBox{nullptr}
    , _rotateSpheresParent{nullptr}
    , _scaleBoxesParent{nullptr}
    , _boundingDimensions{Vector3{1.f, 1.f, 1.f}}
    , _renderObserver{nullptr}
    , _pointerObserver{nullptr}
    , _scaleDragSpeed{0.2f}
    , _tmpVector{Vector3{0.f, 0.f, 0.f}}
{
  // Do not update the gizmo's scale so it has a fixed size to the object its
  // attached to
  _updateScale = false;

  // Create Materials
  auto coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  coloredMaterial->setDisableLighting(true);
  coloredMaterial->emissiveColor = color;
  auto hoverColoredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  hoverColoredMaterial->setDisableLighting(true);
  hoverColoredMaterial->emissiveColor
    = color.copy().add(Color3(0.2f, 0.2f, 0.2f));

  // Build bounding box out of lines
  _lineBoundingBox = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  _lineBoundingBox->setRotationQuaternion(Quaternion());
  vector_t<LinesMesh*> lines;
  vector_t<vector_t<Vector3>> linesPoints{
    {Vector3(0.f, 0.f, 0.f), Vector3(_boundingDimensions.x, 0.f, 0.f)},
    {Vector3(0.f, 0.f, 0.f), Vector3(0.f, _boundingDimensions.y, 0.f)},
    {Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, _boundingDimensions.z)},
    {Vector3(_boundingDimensions.x, 0.f, 0.f),
     Vector3(_boundingDimensions.x, _boundingDimensions.y, 0.f)},
    {Vector3(_boundingDimensions.x, 0.f, 0.f),
     Vector3(_boundingDimensions.x, 0.f, _boundingDimensions.z)},
    {Vector3(0.f, _boundingDimensions.y, 0.f),
     Vector3(_boundingDimensions.x, _boundingDimensions.y, 0.f)},
    {Vector3(0.f, _boundingDimensions.y, 0.f),
     Vector3(0.f, _boundingDimensions.y, _boundingDimensions.z)},
    {Vector3(0.f, 0.f, _boundingDimensions.z),
     Vector3(_boundingDimensions.x, 0.f, _boundingDimensions.z)},
    {Vector3(0.f, 0.f, _boundingDimensions.z),
     Vector3(0.f, _boundingDimensions.y, _boundingDimensions.z)},
    {Vector3(_boundingDimensions.x, _boundingDimensions.y,
             _boundingDimensions.z),
     Vector3(0.f, _boundingDimensions.y, _boundingDimensions.z)},
    {Vector3(_boundingDimensions.x, _boundingDimensions.y,
             _boundingDimensions.z),
     Vector3(_boundingDimensions.x, 0.f, _boundingDimensions.z)},
    {Vector3(_boundingDimensions.x, _boundingDimensions.y,
             _boundingDimensions.z),
     Vector3(_boundingDimensions.x, _boundingDimensions.y, 0.f)},
  };
  for (const auto& linePoints : linesPoints) {
    LinesOptions options;
    options.points = linePoints;
    lines.emplace_back(MeshBuilder::CreateLines(
      "lines", options, gizmoLayer->utilityLayerScene.get()));
  }
  for (auto& l : lines) {
    l->color = color;
    l->position().addInPlace(Vector3(-_boundingDimensions.x / 2.f,
                                     -_boundingDimensions.y / 2.f,
                                     -_boundingDimensions.z / 2.f));
    l->isPickable = false;
    _lineBoundingBox->addChild(l);
  }
  _rootMesh->addChild(_lineBoundingBox);

  // Create rotation spheres
  _rotateSpheresParent
    = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  _rotateSpheresParent->setRotationQuaternion(Quaternion());
  for (unsigned int i = 0; i < 12; ++i) {
    SphereOptions sphereOptions{0.f};
    auto sphere = MeshBuilder::CreateSphere(
      "", sphereOptions, gizmoLayer->utilityLayerScene.get());
    sphere->setRotationQuaternion(Quaternion());
    sphere->material = coloredMaterial;

    // Drag behavior
    PointerDragBehavior _dragBehavior;
    _dragBehavior.moveAttached    = false;
    _dragBehavior.updateDragPlane = false;
    sphere->addBehavior(&_dragBehavior);
    Vector3 startingTurnDirection{1.f, 0.f, 0.f};
    auto totalTurnAmountOfDrag = 0;
    _dragBehavior.onDragStartObservable.add(
      [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
        startingTurnDirection.copyFrom(sphere->forward());
        totalTurnAmountOfDrag = 0;
      });
    _dragBehavior.onDragObservable.add([&](DragMoveEvent* event,
                                           EventState& /*es*/) {
      if (attachedMesh) {
        auto worldDragDirection = startingTurnDirection;

        // Project the world right on to the drag plane
        auto toSub = event->dragPlaneNormal.scale(
          Vector3::Dot(event->dragPlaneNormal, worldDragDirection));
        auto dragAxis = worldDragDirection.subtract(toSub).normalizeToNew();

        // project drag delta on to the resulting drag axis and rotate based on
        // that
        auto projectDist = -Vector3::Dot(dragAxis, event->delta);

        // Rotate based on axis
        if (!attachedMesh()->rotationQuaternion()) {
          attachedMesh()->setRotationQuaternion(
            Quaternion::RotationYawPitchRoll(attachedMesh()->rotation().y,
                                             attachedMesh()->rotation().x,
                                             attachedMesh()->rotation().z));
        }

        // Do not allow the object to turn more than a full circle
        totalTurnAmountOfDrag += projectDist;
        if (::std::abs(totalTurnAmountOfDrag) <= Math::PI2) {
          if (i >= 8) {
            Quaternion::RotationYawPitchRollToRef(0.f, 0.f, projectDist,
                                                  _tmpQuaternion);
          }
          else if (i >= 4) {
            Quaternion::RotationYawPitchRollToRef(projectDist, 0.f, 0.f,
                                                  _tmpQuaternion);
          }
          else {
            Quaternion::RotationYawPitchRollToRef(0.f, projectDist, 0.f,
                                                  _tmpQuaternion);
          }
          attachedMesh()->rotationQuaternion()->multiplyInPlace(_tmpQuaternion);
        }
      }
    });

    // Selection/deselection
    _dragBehavior.onDragStartObservable.add(
      [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
        _selectNode(sphere);
      });
    _dragBehavior.onDragEndObservable.add(
      [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
        _selectNode(nullptr);
      });

    _rotateSpheresParent->addChild(sphere);
  }
  _rootMesh->addChild(_rotateSpheresParent);

  // Create scale cubes
  _scaleBoxesParent
    = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  _scaleBoxesParent->setRotationQuaternion(Quaternion());
  for (unsigned int i = 0; i < 2; i++) {
    for (unsigned int j = 0; j < 2; j++) {
      for (unsigned int k = 0; k < 2; k++) {
        BoxOptions boxOptions{0.1f};
        auto box      = MeshBuilder::CreateBox("", boxOptions,
                                          gizmoLayer->utilityLayerScene.get());
        box->material = coloredMaterial;

        // Dragging logic
        auto dragAxis
          = Vector3(i == 0 ? -1 : 1, j == 0 ? -1 : 1, k == 0 ? -1 : 1);
        PointerDragBehaviorOptions options;
        options.dragAxis = dragAxis;
        PointerDragBehavior _dragBehavior(options);
        _dragBehavior.moveAttached = false;
        box->addBehavior(&_dragBehavior);
        _dragBehavior.onDragObservable.add([&](DragMoveEvent* event,
                                               EventState& /*es*/) {
          if (attachedMesh) {
            // Current boudning box dimensions
            auto boundingInfo = attachedMesh()->getBoundingInfo().boundingBox;
            auto boundBoxDimensions
              = boundingInfo.maximum.subtract(boundingInfo.minimum)
                  .multiplyInPlace(attachedMesh()->scaling());

            // Get the change in bounding box size/2 and add this to the mesh's
            // position to offset from scaling with center pivot point
            auto deltaScale = Vector3(event->dragDistance, event->dragDistance,
                                      event->dragDistance);
            deltaScale.scaleInPlace(_scaleDragSpeed);
            auto scaleRatio
              = deltaScale.divide(attachedMesh()->scaling()).scaleInPlace(0.5f);
            auto moveDirection = boundBoxDimensions.multiply(scaleRatio)
                                   .multiplyInPlace(dragAxis);
            auto worldMoveDirection = Vector3::TransformCoordinates(
              moveDirection,
              attachedMesh()->getWorldMatrix()->getRotationMatrix());

            // Update scale and position
            attachedMesh()->scaling().addInPlace(deltaScale);
            attachedMesh()->getAbsolutePosition().addToRef(worldMoveDirection,
                                                           _tmpVector);
            attachedMesh()->setAbsolutePosition(_tmpVector);
          }
        });

        // Selection/deselection
        _dragBehavior.onDragStartObservable.add(
          [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
            _selectNode(box);
          });
        _dragBehavior.onDragEndObservable.add(
          [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
            _selectNode(nullptr);
          });

        _scaleBoxesParent->addChild(box);
      }
    }
  }
  _rootMesh->addChild(_scaleBoxesParent);

  // Hover color change
  unordered_map_t<int, AbstractMesh*> pointerIds;
  _pointerObserver
    = gizmoLayer->utilityLayerScene.get()->onPointerObservable.add(
      [&](PointerInfo* pointerInfo, EventState& /*es*/) {
        if (!stl_util::contains(pointerIds,
                                (pointerInfo->pointerEvent).pointerId)) {
          const auto changeHoverColor
            = [&](const vector_t<AbstractMesh*>& meshes) {
                for (auto& mesh : meshes) {
                  if (pointerInfo->pickInfo.pickedMesh == mesh) {
                    pointerIds[(pointerInfo->pointerEvent).pointerId] = mesh;
                    mesh->material = hoverColoredMaterial;
                  }
                }
              };
          changeHoverColor(_rotateSpheresParent->getChildMeshes());
          changeHoverColor(_scaleBoxesParent->getChildMeshes());
        }
        else {
          if (stl_util::contains(pointerIds,
                                 (pointerInfo->pointerEvent).pointerId)
              && pointerInfo->pickInfo.pickedMesh
                   != pointerIds[(pointerInfo->pointerEvent).pointerId]) {
            pointerIds[(pointerInfo->pointerEvent).pointerId]->material
              = coloredMaterial;
            pointerIds.erase((pointerInfo->pointerEvent).pointerId);
          }
        }
      });

  // Update bounding box positions
  _renderObserver = gizmoLayer->originalScene->onBeforeRenderObservable.add(
    [&](Scene* /*scene*/, EventState& /*es*/) { _updateBoundingBox(); });
  _updateBoundingBox();
}

BoundingBoxGizmo::~BoundingBoxGizmo()
{
}

void BoundingBoxGizmo::_selectNode(Mesh* selectedMesh)
{
  auto childMeshes = _rotateSpheresParent->getChildMeshes();
  for (auto& m : _scaleBoxesParent->getChildMeshes()) {
    m->isVisible = (!selectedMesh || m == selectedMesh);
  }
}

void BoundingBoxGizmo::_updateBoundingBox()
{
  if (attachedMesh) {
    // Update bounding dimensions/positions
    auto boundingInfo = attachedMesh()->getBoundingInfo().boundingBox;
    auto boundBoxDimensions
      = boundingInfo.maximum.subtract(boundingInfo.minimum)
          .multiplyInPlace(attachedMesh()->scaling());
    _boundingDimensions.copyFrom(boundBoxDimensions);
    _lineBoundingBox->scaling().copyFrom(_boundingDimensions);
  }

  // Update rotation sphere locations
  auto rotateSpheres = _rotateSpheresParent->getChildMeshes();
  for (unsigned int i = 0; i < 3; ++i) {
    for (unsigned int j = 0; j < 2; ++j) {
      for (unsigned int k = 0; k < 2; ++k) {
        auto index = ((i * 4) + (j * 2)) + k;
        if (index < rotateSpheres.size()) {
          if (i == 0) {
            rotateSpheres[index]->position().set(_boundingDimensions.x / 2,
                                                 _boundingDimensions.y * j,
                                                 _boundingDimensions.z * k);
            rotateSpheres[index]->position().addInPlace(
              Vector3(-_boundingDimensions.x / 2, -_boundingDimensions.y / 2,
                      -_boundingDimensions.z / 2));
            rotateSpheres[index]->lookAt(
              Vector3::Cross(Vector3::Right(),
                             rotateSpheres[index]->position().normalizeToNew())
                .normalizeToNew()
                .add(rotateSpheres[index]->position()));
          }
          if (i == 1) {
            rotateSpheres[index]->position().set(_boundingDimensions.x * j,
                                                 _boundingDimensions.y / 2,
                                                 _boundingDimensions.z * k);
            rotateSpheres[index]->position().addInPlace(
              Vector3(-_boundingDimensions.x / 2, -_boundingDimensions.y / 2,
                      -_boundingDimensions.z / 2));
            rotateSpheres[index]->lookAt(
              Vector3::Cross(Vector3::Up(),
                             rotateSpheres[index]->position().normalizeToNew())
                .normalizeToNew()
                .add(rotateSpheres[index]->position()));
          }
          if (i == 2) {
            rotateSpheres[index]->position().set(_boundingDimensions.x * j,
                                                 _boundingDimensions.y * k,
                                                 _boundingDimensions.z / 2);
            rotateSpheres[index]->position().addInPlace(
              Vector3(-_boundingDimensions.x / 2, -_boundingDimensions.y / 2,
                      -_boundingDimensions.z / 2));
            rotateSpheres[index]->lookAt(
              Vector3::Cross(Vector3::Forward(),
                             rotateSpheres[index]->position().normalizeToNew())
                .normalizeToNew()
                .add(rotateSpheres[index]->position()));
          }
        }
      }
    }
  }

  // Update scale box locations
  auto scaleBoxes = _scaleBoxesParent->getChildMeshes();
  for (unsigned int i = 0; i < 2; ++i) {
    for (unsigned int j = 0; j < 2; ++j) {
      for (unsigned int k = 0; k < 2; ++k) {
        auto index = ((i * 4) + (j * 2)) + k;
        if (index < scaleBoxes.size() && scaleBoxes[index]) {
          scaleBoxes[index]->position().set(_boundingDimensions.x * i,
                                            _boundingDimensions.y * j,
                                            _boundingDimensions.z * k);
          scaleBoxes[index]->position().addInPlace(
            Vector3(-_boundingDimensions.x / 2, -_boundingDimensions.y / 2,
                    -_boundingDimensions.z / 2));
        }
      }
    }
  }
}

void BoundingBoxGizmo::setEnabledRotationAxis(const string_t axis)
{
  size_t i = 0;
  for (auto& m : _rotateSpheresParent->getChildMeshes()) {
    if (i < 4) {
      m->setEnabled(String::contains(axis, "x"));
    }
    else if (i < 8) {
      m->setEnabled(String::contains(axis, "y"));
    }
    else {
      m->setEnabled(String::contains(axis, "z"));
    }
    ++i;
  }
}

void BoundingBoxGizmo::dispose(bool doNotRecurse,
                               bool disposeMaterialAndTextures)
{
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  gizmoLayer->originalScene->onBeforeRenderObservable.remove(_renderObserver);
  _lineBoundingBox->dispose();
  _rotateSpheresParent->dispose();
  _scaleBoxesParent->dispose();
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
