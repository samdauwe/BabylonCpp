#include <babylon/gizmos/bounding_box_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/builders/box_builder.h>
#include <babylon/meshes/builders/lines_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/misc/pivot_tools.h>
#include <babylon/misc/string_tools.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

BoundingBoxGizmo::BoundingBoxGizmo(const Color3& color,
                                   const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , ignoreChildren{false}
    , includeChildPredicate{nullptr}
    , rotationSphereSize{0.1f}
    , scaleBoxSize{0.1f}
    , fixedDragMeshScreenSize{false}
    , fixedDragMeshScreenSizeDistanceFactor{10.f}
    , scalePivot{std::nullopt}
    , _lineBoundingBox{nullptr}
    , _rotateSpheresParent{nullptr}
    , _scaleBoxesParent{nullptr}
    , _boundingDimensions{Vector3{1.f, 1.f, 1.f}}
    , _renderObserver{nullptr}
    , _pointerObserver{nullptr}
    , _scaleDragSpeed{0.2f}
    , _tmpVector{Vector3{0.f, 0.f, 0.f}}
    , _anchorMesh{nullptr}
    , _dragMesh{nullptr}
    , pointerDragBehavior{std::make_unique<PointerDragBehavior>()}
    , coloredMaterial{nullptr}
    , hoverColoredMaterial{nullptr}
{
  // Do not update the gizmo's scale so it has a fixed size to the object its attached to
  updateScale = false;

  _anchorMesh = AbstractMesh::New("anchor", iGizmoLayer->utilityLayerScene.get());
  // Create Materials
  coloredMaterial                  = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  coloredMaterial->disableLighting = true;

  hoverColoredMaterial = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  hoverColoredMaterial->disableLighting = true;

  // Build bounding box out of lines
  _lineBoundingBox                     = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  _lineBoundingBox->rotationQuaternion = Quaternion();
  std::vector<LinesMeshPtr> lines;
  std::vector<std::vector<Vector3>> linesPoints{
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
    {Vector3(_boundingDimensions.x, _boundingDimensions.y, _boundingDimensions.z),
     Vector3(0.f, _boundingDimensions.y, _boundingDimensions.z)},
    {Vector3(_boundingDimensions.x, _boundingDimensions.y, _boundingDimensions.z),
     Vector3(_boundingDimensions.x, 0.f, _boundingDimensions.z)},
    {Vector3(_boundingDimensions.x, _boundingDimensions.y, _boundingDimensions.z),
     Vector3(_boundingDimensions.x, _boundingDimensions.y, 0.f)},
  };
  for (const auto& linePoints : linesPoints) {
    LinesOptions options;
    options.points = linePoints;
    lines.emplace_back(
      LinesBuilder::CreateLines("lines", options, gizmoLayer->utilityLayerScene.get()));
  }
  for (const auto& l : lines) {
    l->color = color;
    l->position().addInPlace(Vector3(-_boundingDimensions.x / 2.f, -_boundingDimensions.y / 2.f,
                                     -_boundingDimensions.z / 2.f));
    l->isPickable = false;
    _lineBoundingBox->addChild(*l);
  }
  _rootMesh->addChild(*_lineBoundingBox);

  setColor(color);

  // Create rotation spheres
  _rotateSpheresParent = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  _rotateSpheresParent->rotationQuaternion = Quaternion();
  for (unsigned int i = 0; i < 12; ++i) {
    SphereOptions sphereOptions;
    sphereOptions.diameter = 1.f;
    auto sphere
      = SphereBuilder::CreateSphere("", sphereOptions, gizmoLayer->utilityLayerScene.get());
    sphere->rotationQuaternion = Quaternion();
    sphere->material           = coloredMaterial;

    // Drag behavior
    PointerDragBehavior _dragBehavior;
    _dragBehavior.moveAttached    = false;
    _dragBehavior.updateDragPlane = false;
    // sphere->addBehavior(&_dragBehavior);
    startingTurnDirection = Vector3{1.f, 0.f, 0.f};
    totalTurnAmountOfDrag = 0.f;
    _dragBehavior.onDragStartObservable.add(
      [this, &sphere](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
        startingTurnDirection.copyFrom(sphere->forward());
        totalTurnAmountOfDrag = 0.f;
      });
    _dragBehavior.onDragObservable.add([this, i](DragMoveEvent* event, EventState& /*es*/) {
      onRotationSphereDragObservable.notifyObservers({});
      if (attachedMesh()) {
        auto originalParent = attachedMesh()->parent();
        if (originalParent) {
          auto _originalParent = static_cast<Mesh*>(originalParent);
          if (_originalParent->scaling().isNonUniformWithinEpsilon(0.001f)) {
            BABYLON_LOG_WARN("BoundingBoxGizmo",
                             "BoundingBoxGizmo controls are not supported on child meshes with "
                             "non-uniform parent scaling")
            return;
          }
        }
        PivotTools::_RemoveAndStorePivotPoint(attachedMesh);

        auto worldDragDirection = startingTurnDirection;

        // Project the world right on to the drag plane
        auto toSub
          = event->dragPlaneNormal.scale(Vector3::Dot(event->dragPlaneNormal, worldDragDirection));
        auto dragAxis = worldDragDirection.subtract(toSub).normalizeToNew();

        // project drag delta on to the resulting drag axis and rotate based
        // on that
        auto projectDist = Vector3::Dot(dragAxis, event->delta) < 0.f ?
                             std::abs(event->delta.length()) :
                             -std::abs(event->delta.length());

        // Make rotation relative to size of mesh.
        projectDist
          = (projectDist / _boundingDimensions.length()) * _anchorMesh->scaling().length();

        // Rotate based on axis
        if (!attachedMesh()->rotationQuaternion()) {
          attachedMesh()->rotationQuaternion = Quaternion::RotationYawPitchRoll(
            attachedMesh()->rotation().y, attachedMesh()->rotation().x,
            attachedMesh()->rotation().z);
        }
        if (!_anchorMesh->rotationQuaternion()) {
          _anchorMesh->rotationQuaternion = Quaternion::RotationYawPitchRoll(
            _anchorMesh->rotation().y, _anchorMesh->rotation().x, _anchorMesh->rotation().z);
        }

        // Do not allow the object to turn more than a full circle
        totalTurnAmountOfDrag += projectDist;
        if (std::abs(totalTurnAmountOfDrag) <= Math::PI2) {
          if (i >= 8) {
            Quaternion::RotationYawPitchRollToRef(0.f, 0.f, projectDist, _tmpQuaternion);
          }
          else if (i >= 4) {
            Quaternion::RotationYawPitchRollToRef(projectDist, 0.f, 0.f, _tmpQuaternion);
          }
          else {
            Quaternion::RotationYawPitchRollToRef(0.f, projectDist, 0.f, _tmpQuaternion);
          }

          // Rotate around center of bounding box
          _anchorMesh->addChild(*attachedMesh());
          _anchorMesh->rotationQuaternion()->multiplyToRef(_tmpQuaternion,
                                                           *_anchorMesh->rotationQuaternion());
          _anchorMesh->removeChild(*attachedMesh());
          attachedMesh()->setParent(originalParent);
        }
        updateBoundingBox();

        PivotTools::_RestorePivotPoint(attachedMesh());
      }
      _updateDummy();
    });

    // Selection/deselection
    _dragBehavior.onDragStartObservable.add(
      [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
        onDragStartObservable.notifyObservers(nullptr);
        _selectNode(sphere);
      });
    _dragBehavior.onDragEndObservable.add([&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
      onRotationSphereDragEndObservable.notifyObservers(nullptr);
      _selectNode(nullptr);
      _updateDummy();
    });

    _rotateSpheresParent->addChild(*sphere);
  }
  _rootMesh->addChild(*_rotateSpheresParent);

  // Create scale cubes
  _scaleBoxesParent = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());
  _scaleBoxesParent->rotationQuaternion = Quaternion();
  for (unsigned int i = 0; i < 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      for (unsigned int k = 0; k < 3; ++k) {
        // create box for relevant axis
        const auto zeroAxisCount = ((i == 1) ? 1 : 0) + ((j == 1) ? 1 : 0) + ((k == 1) ? 1 : 0);
        if (zeroAxisCount == 1 || zeroAxisCount == 3) {
          continue;
        }

        BoxOptions boxOptions;
        boxOptions.size = 0.1f;
        auto box      = BoxBuilder::CreateBox("", boxOptions, gizmoLayer->utilityLayerScene.get());
        box->material = coloredMaterial;
        box->metadata = zeroAxisCount == 2; // None homogenous scale handle

        // Dragging logic
        auto dragAxis = Vector3(i - 1, j - 1, k - 1);
        PointerDragBehaviorOptions options;
        options.dragAxis = dragAxis;
        PointerDragBehavior _dragBehavior(options);
        _dragBehavior.moveAttached = false;
        // box->addBehavior(&_dragBehavior);
        _dragBehavior.onDragObservable.add([&](DragMoveEvent* event, EventState& /*es*/) {
          onScaleBoxDragObservable.notifyObservers(nullptr);
          if (attachedMesh()) {
            auto originalParent = attachedMesh()->parent();
            if (originalParent) {
              auto _originalParent = static_cast<Mesh*>(originalParent);
              if (_originalParent->scaling().isNonUniformWithinEpsilon(0.001f)) {
                BABYLON_LOG_WARN("BoundingBoxGizmo",
                                 "BoundingBoxGizmo controls are not supported on child meshes with "
                                 "non-uniform parent scaling")
                return;
              }
            }
            PivotTools::_RemoveAndStorePivotPoint(attachedMesh);
            auto relativeDragDistance = (event->dragDistance / _boundingDimensions.length())
                                        * _anchorMesh->scaling().length();
            Vector3 deltaScale(relativeDragDistance, relativeDragDistance, relativeDragDistance);
            if (zeroAxisCount == 2) {
              // scale on 1 axis when using the anchor box in the face middle
              deltaScale.x *= std::abs(dragAxis.x);
              deltaScale.y *= std::abs(dragAxis.y);
              deltaScale.z *= std::abs(dragAxis.z);
            }
            deltaScale.scaleInPlace(_scaleDragSpeed);
            updateBoundingBox();
            if (scalePivot) {
              attachedMesh()->getWorldMatrix().getRotationMatrixToRef(_tmpRotationMatrix);
              // Move anchor to desired pivot point (Bottom left corner + dimension/2)
              _boundingDimensions.scaleToRef(0.5f, _tmpVector);
              Vector3::TransformCoordinatesToRef(_tmpVector, _tmpRotationMatrix, _tmpVector);
              _anchorMesh->position().subtractInPlace(_tmpVector);
              _boundingDimensions.multiplyToRef(*scalePivot, _tmpVector);
              Vector3::TransformCoordinatesToRef(_tmpVector, _tmpRotationMatrix, _tmpVector);
              _anchorMesh->position().addInPlace(_tmpVector);
            }
            else {
              // Scale from the position of the opposite corner
              box->absolutePosition().subtractToRef(_anchorMesh->position(), _tmpVector);
              _anchorMesh->position().subtractInPlace(_tmpVector);
            }

            _anchorMesh->addChild(*attachedMesh());
            _anchorMesh->scaling().addInPlace(deltaScale);
            if (_anchorMesh->scaling().x < 0.f || _anchorMesh->scaling().y < 0.f
                || _anchorMesh->scaling().z < 0.f) {
              _anchorMesh->scaling().subtractInPlace(deltaScale);
            }
            _anchorMesh->removeChild(*attachedMesh());

            attachedMesh()->setParent(originalParent);
            PivotTools::_RestorePivotPoint(attachedMesh());
          }
          _updateDummy();
        });

        // Selection/deselection
        _dragBehavior.onDragStartObservable.add(
          [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
            onDragStartObservable.notifyObservers(nullptr);
            _selectNode(box);
          });
        _dragBehavior.onDragEndObservable.add(
          [this](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
            onScaleBoxDragEndObservable.notifyObservers(nullptr);
            _selectNode(nullptr);
            _updateDummy();
          });

        _scaleBoxesParent->addChild(*box);
      }
    }
  }
  _rootMesh->addChild(*_scaleBoxesParent);

  // Hover color change
  std::unordered_map<int, AbstractMeshPtr> pointerIds;
  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (!stl_util::contains(pointerIds, (pointerInfo->pointerEvent).pointerId)) {
        const auto changeHoverColor = [&](const std::vector<AbstractMeshPtr>& meshes) {
          for (auto& mesh : meshes) {
            if (pointerInfo->pickInfo.pickedMesh == mesh) {
              pointerIds[(pointerInfo->pointerEvent).pointerId] = mesh;
              mesh->material                                    = hoverColoredMaterial;
            }
          }
        };
        changeHoverColor(_rotateSpheresParent->getChildMeshes());
        changeHoverColor(_scaleBoxesParent->getChildMeshes());
      }
      else {
        if (stl_util::contains(pointerIds, (pointerInfo->pointerEvent).pointerId)
            && pointerInfo->pickInfo.pickedMesh
                 != pointerIds[(pointerInfo->pointerEvent).pointerId]) {
          pointerIds[(pointerInfo->pointerEvent).pointerId]->material = coloredMaterial;
          pointerIds.erase((pointerInfo->pointerEvent).pointerId);
        }
      }
    });

  // Update bounding box positions
  _renderObserver = gizmoLayer->originalScene->onBeforeRenderObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) {
      // Only update the bouding box if scaling has changed
      if (attachedMesh() && !_existingMeshScale.equals(attachedMesh()->scaling())) {
        updateBoundingBox();
      }
      else if (fixedDragMeshScreenSize) {
        _updateRotationSpheres();
        _updateScaleBoxes();
      }

      // If dragg mesh is enabled and dragging, update the attached mesh pose to match the drag mesh
      if (_dragMesh && attachedMesh() && pointerDragBehavior->dragging) {
        _lineBoundingBox->position().rotateByQuaternionToRef(*_rootMesh->rotationQuaternion(),
                                                             _tmpVector);
        attachedMesh()->setAbsolutePosition(_dragMesh->position().add(_tmpVector.scale(-1.f)));
      }
    });
  updateBoundingBox();
}

BoundingBoxGizmo::~BoundingBoxGizmo() = default;

void BoundingBoxGizmo::setColor(const Color3& color)
{
  coloredMaterial->emissiveColor      = color;
  hoverColoredMaterial->emissiveColor = color.copy().add(Color3(0.3f, 0.3f, 0.3f));
  for (const auto& l : _lineBoundingBox->getChildren()) {
    auto lineMesh = std::static_pointer_cast<LinesMesh>(l);
    if (lineMesh) {
      lineMesh->color = color;
    }
  }
}

void BoundingBoxGizmo::_attachedMeshChanged(const AbstractMeshPtr& value)
{
  if (value) {
    // Reset anchor mesh to match attached mesh's scale
    // This is needed to avoid invalid box/sphere position on first drag
    PivotTools::_RemoveAndStorePivotPoint(value);
    auto originalParent = value->parent();
    _anchorMesh->addChild(*value);
    _anchorMesh->removeChild(*value);
    value->setParent(originalParent);
    PivotTools::_RestorePivotPoint(value);
    updateBoundingBox();
    for (const auto& m : value->getChildMeshes(false)) {
      m->markAsDirty("scaling");
    }

    gizmoLayer->utilityLayerScene->onAfterRenderObservable.addOnce(
      [this](Scene* /*scene*/, EventState & /*es*/) -> void { _updateDummy(); });
  }
}

void BoundingBoxGizmo::_selectNode(const MeshPtr& selectedMesh)
{
  auto childMeshes = _rotateSpheresParent->getChildMeshes();
  for (const auto& m : _scaleBoxesParent->getChildMeshes()) {
    m->isVisible = (!selectedMesh || m == selectedMesh);
  }
}

void BoundingBoxGizmo::updateBoundingBox()
{
  if (attachedMesh()) {
    PivotTools::_RemoveAndStorePivotPoint(attachedMesh);

    // Store original parent
    auto originalParent = attachedMesh()->parent();
    attachedMesh()->setParent(nullptr);

    // Store original skelton override mesh
    AbstractMeshPtr originalSkeletonOverrideMesh = nullptr;
    if (attachedMesh()->skeleton()) {
      originalSkeletonOverrideMesh             = attachedMesh()->skeleton()->overrideMesh;
      attachedMesh()->skeleton()->overrideMesh = nullptr;
    }

    _update();

    // Rotate based on axis
    if (!attachedMesh()->rotationQuaternion()) {
      attachedMesh()->rotationQuaternion = Quaternion::RotationYawPitchRoll(
        attachedMesh()->rotation().y, attachedMesh()->rotation().x, attachedMesh()->rotation().z);
    }
    if (!_anchorMesh->rotationQuaternion()) {
      _anchorMesh->rotationQuaternion = Quaternion::RotationYawPitchRoll(
        _anchorMesh->rotation().y, _anchorMesh->rotation().x, _anchorMesh->rotation().z);
    }
    _anchorMesh->rotationQuaternion()->copyFrom(*attachedMesh()->rotationQuaternion());

    // Store original position and reset mesh to origin before computing the bounding box
    _tmpQuaternion.copyFrom(*attachedMesh()->rotationQuaternion());
    _tmpVector.copyFrom(attachedMesh()->position());
    attachedMesh()->rotationQuaternion()->set(0.f, 0.f, 0.f, 1.f);
    attachedMesh()->position().set(0.f, 0.f, 0.f);

    // Update bounding dimensions/positions
    auto boundingMinMax
      = attachedMesh()->getHierarchyBoundingVectors(!ignoreChildren, includeChildPredicate);
    boundingMinMax.max.subtractToRef(boundingMinMax.min, _boundingDimensions);

    // Update gizmo to match bounding box scaling and rotation
    // The position set here is the offset from the origin for the boundingbox when the attached
    // mesh is at the origin The position of the gizmo is then set to the attachedMesh in
    // gizmo._update
    _lineBoundingBox->scaling().copyFrom(_boundingDimensions);
    _lineBoundingBox->position().set((boundingMinMax.max.x + boundingMinMax.min.x) / 2.f,
                                     (boundingMinMax.max.y + boundingMinMax.min.y) / 2.f,
                                     (boundingMinMax.max.z + boundingMinMax.min.z) / 2.f);
    _rotateSpheresParent->position().copyFrom(_lineBoundingBox->position());
    _scaleBoxesParent->position().copyFrom(_lineBoundingBox->position());
    _lineBoundingBox->computeWorldMatrix();
    _anchorMesh->position().copyFrom(_lineBoundingBox->absolutePosition());

    // Restore position/rotation values
    attachedMesh()->rotationQuaternion()->copyFrom(_tmpQuaternion);
    attachedMesh()->position().copyFrom(_tmpVector);

    // Restore original parent
    attachedMesh()->setParent(originalParent);

    // Restore original skeleton override mesh
    if (attachedMesh()->skeleton()) {
      attachedMesh()->skeleton()->overrideMesh = originalSkeletonOverrideMesh;
    }
  }

  _updateRotationSpheres();
  _updateScaleBoxes();

  if (attachedMesh()) {
    _existingMeshScale.copyFrom(attachedMesh()->scaling());
    PivotTools::_RestorePivotPoint(attachedMesh);
  }
}

void BoundingBoxGizmo::_updateRotationSpheres()
{
  // Update rotation sphere locations
  auto rotateSpheres = _rotateSpheresParent->getChildMeshes();
  for (unsigned int i = 0; i < 3; ++i) {
    for (unsigned int j = 0; j < 2; ++j) {
      for (unsigned int k = 0; k < 2; ++k) {
        auto index = ((i * 4) + (j * 2)) + k;
        if (index < rotateSpheres.size()) {
          if (i == 0) {
            rotateSpheres[index]->position().set(
              _boundingDimensions.x / 2.f, _boundingDimensions.y * j, _boundingDimensions.z * k);
            rotateSpheres[index]->position().addInPlace(Vector3(-_boundingDimensions.x / 2.f,
                                                                -_boundingDimensions.y / 2.f,
                                                                -_boundingDimensions.z / 2.f));
            rotateSpheres[index]->lookAt(
              Vector3::Cross(rotateSpheres[index]->position().normalizeToNew(), Vector3::Right())
                .normalizeToNew()
                .add(rotateSpheres[index]->position));
          }
          if (i == 1) {
            rotateSpheres[index]->position().set(
              _boundingDimensions.x * j, _boundingDimensions.y / 2.f, _boundingDimensions.z * k);
            rotateSpheres[index]->position().addInPlace(Vector3(-_boundingDimensions.x / 2.f,
                                                                -_boundingDimensions.y / 2.f,
                                                                -_boundingDimensions.z / 2.f));
            rotateSpheres[index]->lookAt(
              Vector3::Cross(rotateSpheres[index]->position().normalizeToNew(), Vector3::Up())
                .normalizeToNew()
                .add(rotateSpheres[index]->position));
          }
          if (i == 2) {
            rotateSpheres[index]->position().set(
              _boundingDimensions.x * j, _boundingDimensions.y * k, _boundingDimensions.z / 2.f);
            rotateSpheres[index]->position().addInPlace(Vector3(-_boundingDimensions.x / 2.f,
                                                                -_boundingDimensions.y / 2.f,
                                                                -_boundingDimensions.z / 2.f));
            rotateSpheres[index]->lookAt(
              Vector3::Cross(rotateSpheres[index]->position().normalizeToNew(), Vector3::Forward())
                .normalizeToNew()
                .add(rotateSpheres[index]->position));
          }
          if (fixedDragMeshScreenSize && gizmoLayer->utilityLayerScene->activeCamera()) {
            rotateSpheres[index]->absolutePosition().subtractToRef(
              gizmoLayer->utilityLayerScene->activeCamera()->position(), _tmpVector);
            auto distanceFromCamera
              = rotationSphereSize * _tmpVector.length() / fixedDragMeshScreenSizeDistanceFactor;
            rotateSpheres[index]->scaling().set(distanceFromCamera, distanceFromCamera,
                                                distanceFromCamera);
          }
          else {
            rotateSpheres[index]->scaling().set(rotationSphereSize, rotationSphereSize,
                                                rotationSphereSize);
          }
        }
      }
    }
  }
}

void BoundingBoxGizmo::_updateScaleBoxes()
{
  // Update scale box locations
  auto scaleBoxes = _scaleBoxesParent->getChildMeshes();
  auto index      = 0ull;
  for (unsigned int i = 0; i < 3; ++i) {
    for (unsigned int j = 0; j < 3; ++j) {
      for (unsigned int k = 0; k < 3; ++k) {
        const auto zeroAxisCount = ((i == 1) ? 1 : 0) + ((j == 1) ? 1 : 0) + ((k == 1) ? 1 : 0);
        if (zeroAxisCount == 1 || zeroAxisCount == 3) {
          continue;
        }
        if (index < scaleBoxes.size() && scaleBoxes[index]) {
          scaleBoxes[index]->position().set(_boundingDimensions.x * (i / 2.f),
                                            _boundingDimensions.y * (j / 2.f),
                                            _boundingDimensions.z * (k / 2.f));
          scaleBoxes[index]->position().addInPlace(Vector3(-_boundingDimensions.x / 2.f,
                                                           -_boundingDimensions.y / 2.f,
                                                           -_boundingDimensions.z / 2.f));
          if (fixedDragMeshScreenSize && gizmoLayer->utilityLayerScene->activeCamera()) {
            scaleBoxes[index]->absolutePosition().subtractToRef(
              gizmoLayer->utilityLayerScene->activeCamera()->position(), _tmpVector);
            auto distanceFromCamera
              = scaleBoxSize * _tmpVector.length() / fixedDragMeshScreenSizeDistanceFactor;
            scaleBoxes[index]->scaling().set(distanceFromCamera, distanceFromCamera,
                                             distanceFromCamera);
          }
          else {
            scaleBoxes[index]->scaling().set(scaleBoxSize, scaleBoxSize, scaleBoxSize);
          }
        }
        ++index;
      }
    }
  }
}

void BoundingBoxGizmo::setEnabledRotationAxis(const std::string& axis)
{
  size_t i = 0;
  for (auto& m : _rotateSpheresParent->getChildMeshes()) {
    if (i < 4) {
      m->setEnabled(StringTools::contains(axis, "x"));
    }
    else if (i < 8) {
      m->setEnabled(StringTools::contains(axis, "y"));
    }
    else {
      m->setEnabled(StringTools::contains(axis, "z"));
    }
    ++i;
  }
}

void BoundingBoxGizmo::setEnabledScaling(bool enable, bool homogeneousScaling)
{
  for (const auto& m : _scaleBoxesParent->getChildMeshes()) {
    auto enableMesh = enable;
    // Disable heterogenous scale handles if requested.
    if (homogeneousScaling && m->metadata == true) {
      enableMesh = false;
    }
    m->setEnabled(enableMesh);
  }
}

void BoundingBoxGizmo::_updateDummy()
{
  if (_dragMesh) {
    _dragMesh->position().copyFrom(_lineBoundingBox->getAbsolutePosition());
    _dragMesh->scaling().copyFrom(_lineBoundingBox->scaling);
    _dragMesh->rotationQuaternion()->copyFrom(*_rootMesh->rotationQuaternion());
  }
}

void BoundingBoxGizmo::enableDragBehavior()
{
  _dragMesh                     = Mesh::CreateBox("dummy", 1, gizmoLayer->utilityLayerScene.get());
  _dragMesh->visibility         = 0.f;
  _dragMesh->rotationQuaternion = Quaternion();
  pointerDragBehavior->useObjectOrientationForDragging = false;
  // _dragMesh->addBehavior(pointerDragBehavior);
}

void BoundingBoxGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  gizmoLayer->originalScene->onBeforeRenderObservable.remove(_renderObserver);
  _lineBoundingBox->dispose();
  _rotateSpheresParent->dispose();
  _scaleBoxesParent->dispose();
  if (_dragMesh) {
    _dragMesh->dispose();
  }
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

MeshPtr BoundingBoxGizmo::MakeNotPickableAndWrapInBoundingBox(Mesh* mesh)
{
  std::function<void(AbstractMesh * root)> makeNotPickable;
  makeNotPickable = [&makeNotPickable](AbstractMesh* root) {
    root->isPickable = false;
    for (auto& c : root->getChildMeshes()) {
      makeNotPickable(c.get());
    }
  };
  makeNotPickable(mesh);

  // Reset position to get boudning box from origin with no rotation
  if (!mesh->rotationQuaternion()) {
    mesh->rotationQuaternion = Quaternion::RotationYawPitchRoll(
      mesh->rotation().y, mesh->rotation().x, mesh->rotation().z);
  }
  auto oldPos = mesh->position();
  auto oldRot = *mesh->rotationQuaternion();
  mesh->rotationQuaternion()->set(0.f, 0.f, 0.f, 1.f);
  mesh->position().set(0.f, 0.f, 0.f);

  // Update bounding dimensions/positions
  BoxOptions options;
  options.size        = 1.f;
  auto box            = BoxBuilder::CreateBox("box", options, mesh->getScene());
  auto boundingMinMax = mesh->getHierarchyBoundingVectors();
  boundingMinMax.max.subtractToRef(boundingMinMax.min, box->scaling());

  // Adjust scale to avoid undefined behavior when adding child
  if (box->scaling().y == 0.f) {
    box->scaling().y = Math::Epsilon;
  }
  if (box->scaling().x == 0.f) {
    box->scaling().x = Math::Epsilon;
  }
  if (box->scaling().z == 0.f) {
    box->scaling().z = Math::Epsilon;
  }

  box->position().set((boundingMinMax.max.x + boundingMinMax.min.x) / 2.f,
                      (boundingMinMax.max.y + boundingMinMax.min.y) / 2.f,
                      (boundingMinMax.max.z + boundingMinMax.min.z) / 2.f);

  // Restore original positions
  mesh->addChild(*box);
  mesh->rotationQuaternion()->copyFrom(oldRot);
  mesh->position().copyFrom(oldPos);

  // Reverse parenting
  mesh->removeChild(*box);

  box->addChild(*mesh);
  box->visibility = 0.f;
  return box;
}

void BoundingBoxGizmo::setCustomMesh(const MeshPtr& /*mesh*/, bool /*useGizmoMaterial*/)
{
  BABYLON_LOG_ERROR("BoundingBoxGizmo", "Custom meshes are not supported on this gizmo")
}

} // end of namespace BABYLON
