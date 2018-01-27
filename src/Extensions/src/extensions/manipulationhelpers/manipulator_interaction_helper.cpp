#include <babylon/extensions/manipulationhelpers/manipulator_interaction_helper.h>

#include <babylon/cameras/camera.h>
#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/math/vector4.h>
#include <babylon/mesh/abstract_mesh.h>

namespace BABYLON {
namespace Extensions {

ManipulatorInteractionHelper::ManipulatorInteractionHelper(Scene* scene)
    : noPreventContextMenu{false}
    , _flags{ManFlags::None}
    , _shiftKeyState{false}
    , _scene{scene}
    , _rotationFactor{1.f}
    , _radix{Radix{scene}}
{
  _scene->onBeforeRenderObservable.add(
    [this](Scene* s, EventState& e) { onBeforeRender(s, e); });
  _scene->onPointerObservable.add(
    [this](PointerInfo* e, EventState& s) { onPointer(*e, s); }, -1, true);
}

ManipulatorInteractionHelper::~ManipulatorInteractionHelper()
{
}

void ManipulatorInteractionHelper::attachManipulatedNode(Node* node)
{
  _manipulatedNode = node;
  _radix.show();
}

void ManipulatorInteractionHelper::detachManipulatedNode(Node* /*node*/)
{
  _manipulatedNode = nullptr;
  _radix.hide();
}

void ManipulatorInteractionHelper::onBeforeRender(Scene* /*scene*/,
                                                  EventState& /*state*/)
{
  renderManipulator();
}

void ManipulatorInteractionHelper::onPointer(PointerInfo& e, EventState& state)
{
  if (!_manipulatedNode) {
    return;
  }

  auto rayPos        = getRayPosition(e.pointerEvent);
  auto shiftKeyState = e.pointerEvent.shiftKey;

  // Detect Modifier Key changes for shift while manipulating: commit and start
  // a new manipulation
  if (hasManFlags(ManFlags::DragMode) && shiftKeyState != _shiftKeyState) {
    beginDrag(rayPos, e.pointerEvent);
  }

  // Mouse move
  if (e.type == PointerEventTypes::POINTERMOVE) {

    // Right button release while left is down => cancel the manipulation. only
    // processed when the context menu is not showed during manipulation
    if (!noPreventContextMenu && e.pointerEvent.button == MouseButtonType::RIGHT
        && e.pointerEvent.buttons == 1) {
      setManipulatedNodeWorldMatrix(_firstTransform);
      setManFlags(ManFlags::Exiting);
    }

    else if (hasManFlags(ManFlags::DragMode)
             && !hasManFlags(ManFlags::Exiting)) {
      state.skipNextObservers = true;

      if (shiftKeyState || hasManipulatedMode(RadixFeatures::Rotations)) {
        doRot(rayPos);
      }
      else {
        doPos(rayPos);
      }
    }
    else {
      _radix.setHighlighted(_radix.intersect(rayPos));
    }
  }

  // Left button down
  else if (e.type == PointerEventTypes::POINTERDOWN
           && e.pointerEvent.button == MouseButtonType::LEFT) {
    _manipulatedMode = _radix.intersect(rayPos);

    if (_manipulatedMode != RadixFeatures::None) {
      state.skipNextObservers = true;
      beginDrag(rayPos, e.pointerEvent);

      if (hasManipulatedMode(RadixFeatures::Rotations)) {
        doRot(rayPos);
      }
      else {
        doPos(rayPos);
      }
    }
  }

  else if (e.type == PointerEventTypes::POINTERUP) {
    if (hasManFlags(ManFlags::DragMode)) {
      state.skipNextObservers = true;
    }
    _radix.setHighlighted(_radix.intersect(rayPos));

    // Left up: end manipulation
    if (e.pointerEvent.button == MouseButtonType::LEFT) {
      endDragMode();
    }
  }
}

void ManipulatorInteractionHelper::beginDrag(const Vector2& rayPos,
                                             const PointerEvent& event)
{
  _firstMousePos = rayPos;
  _prevMousePos  = _firstMousePos;
  _shiftKeyState = event.shiftKey;

  auto mtx = *getManipulatedNodeWorldMatrix();
  _pos     = mtx.getTranslation();
  _right   = (*mtx.getRow(0)).toVector3();
  _up      = (*mtx.getRow(1)).toVector3();
  _view    = (*mtx.getRow(2)).toVector3();

  _oldPos         = _pos;
  _firstTransform = mtx;
  _flags |= ManFlags::FirstHit | ManFlags::DragMode;
}

void ManipulatorInteractionHelper::endDragMode()
{
  clearManFlags(ManFlags::DragMode | ManFlags::Exiting);
}

void ManipulatorInteractionHelper::doRot(const Vector2& rayPos)
{
  if (hasManFlags(ManFlags::FirstHit)) {
    clearManFlags(ManFlags::FirstHit);
    return;
  }

  const float dx = rayPos.x - _prevMousePos.x;
  const float dy = rayPos.y - _prevMousePos.y;

  const auto cr = _scene->getEngine()->getRenderingCanvasClientRect();

  float ax = (dx / (*cr).width) * Math::PI2 * _rotationFactor;
  float ay = (dy / (*cr).height) * Math::PI2 * _rotationFactor;

  if (rotationStep != 0.f) {
    float rem = std::fmod(ax, rotationStep);
    ax -= rem;

    rem = std::fmod(ay, rotationStep);
    ay -= rem;
  }

  auto mtx = Matrix::Identity();

  if (hasManipulatedMode(RadixFeatures::ArrowX | RadixFeatures::RotationX)) {
    mtx = Matrix::RotationX(ay);
  }

  else if (hasManipulatedMode(RadixFeatures::ArrowY
                              | RadixFeatures::RotationY)) {
    mtx = Matrix::RotationY(ay);
  }

  else if (hasManipulatedMode(RadixFeatures::ArrowZ
                              | RadixFeatures::RotationZ)) {
    mtx = Matrix::RotationZ(ay);
  }

  else {
    if (hasManipulatedMode(
          /*RadixFeatures::CenterSquare |*/ RadixFeatures::PlaneSelectionXY
          | RadixFeatures::PlaneSelectionXZ)) {
      mtx = mtx.multiply(Matrix::RotationX(ay));
    }

    if (hasManipulatedMode(RadixFeatures::PlaneSelectionXY
                           | RadixFeatures::PlaneSelectionYZ)) {
      mtx = mtx.multiply(Matrix::RotationY(ax));
    }

    if (hasManipulatedMode(RadixFeatures::PlaneSelectionXZ)) {
      mtx = mtx.multiply(Matrix::RotationZ(ay));
    }

    if (hasManipulatedMode(
          /*RadixFeatures::CenterSquare |*/ RadixFeatures::PlaneSelectionXZ)) {
      mtx = mtx.multiply(Matrix::RotationZ(ax));
    }
  }

  auto tmtx = mtx.multiply(_firstTransform);
  setManipulatedNodeWorldMatrix(tmtx);
}

void ManipulatorInteractionHelper::doPos(const Vector2& rayPos)
{
  auto v        = Vector3::Zero();
  auto identity = std::make_unique<Matrix>(Matrix::Identity());
  auto ray      = _scene->createPickingRay(static_cast<int>(rayPos.x),
                                      static_cast<int>(rayPos.y),
                                      identity.get(), _scene->activeCamera);

  if (hasManipulatedMode(RadixFeatures::PlaneSelectionXY
                         | RadixFeatures::PlaneSelectionXZ
                         | RadixFeatures::PlaneSelectionYZ)) {
    Plane pl0;
    Vector3 hit;

    if (hasManipulatedMode(RadixFeatures::PlaneSelectionXY)) {
      pl0 = Plane::FromPoints(_pos, _pos.add(_right), _pos.add(_up));
    }
    else if (hasManipulatedMode(RadixFeatures::PlaneSelectionXZ)) {
      pl0 = Plane::FromPoints(_pos, _pos.add(_right), _pos.add(_view));
    }
    else if (hasManipulatedMode(RadixFeatures::PlaneSelectionYZ)) {
      pl0 = Plane::FromPoints(_pos, _pos.add(_up), _pos.add(_view));
    }
    else {
      // TODO Exception
    }

    float clip = 0.06f;

    // Check if the plane is too parallel to the ray
    if (std::abs(Vector3::Dot(pl0.normal, ray->direction)) < clip) {
      return;
    }

    // Make the intersection
    auto distance = ray->intersectsPlane(pl0);
    hit = ManipulatorInteractionHelper::ComputeRayHit(*ray, *distance);

    // Check if it's the first call
    if (hasManFlags(ManFlags::FirstHit)) {
      _flags &= ~ManFlags::FirstHit;
      _prevHit = hit;
      return;
    }

    // Compute the vector
    v = hit.subtract(_prevHit);
  }

  else if ((_manipulatedMode
            & (RadixFeatures::ArrowX | RadixFeatures::ArrowY
               | RadixFeatures::ArrowZ))
           != RadixFeatures::None) {
    Plane pl0, pl1;
    Vector3 hit;
    float s;

    if (hasManFlags(ManFlags::FirstHit)) {
      std::tie(pl0, pl1) = setupIntersectionPlanes(_manipulatedMode);

      if (std::abs(Vector3::Dot(pl0.normal, ray->direction))
          > std::abs(Vector3::Dot(pl1.normal, ray->direction))) {
        auto distance = ray->intersectsPlane(pl0);
        hit = ManipulatorInteractionHelper::ComputeRayHit(*ray, *distance);
        auto number = ~ManFlags::Plane2;
        _flags &= number;
      }
      else {
        auto distance = ray->intersectsPlane(pl1);
        hit = ManipulatorInteractionHelper::ComputeRayHit(*ray, *distance);
        _flags |= ManFlags::Plane2;
      }

      _flags &= ~ManFlags::FirstHit;
      _prevHit = hit;
      return;
    }
    else {
      Vector3 axis;
      std::tie(pl0, axis) = setupIntersectionPlane(
        _manipulatedMode, hasManFlags(ManFlags::Plane2));

      auto distance = ray->intersectsPlane(pl0);
      hit = ManipulatorInteractionHelper::ComputeRayHit(*ray, *distance);
      v   = hit.subtract(_prevHit);
      s   = Vector3::Dot(axis, v);
      v   = axis.multiplyByFloats(s, s, s);
    }
  }

  if (translationStep != 0.f) {
    v.x -= std::fmod(v.x, translationStep);
    v.y -= std::fmod(v.y, translationStep);
    v.z -= std::fmod(v.z, translationStep);
  }

  auto mtx = _firstTransform;
  mtx.setTranslation(mtx.getTranslation().add(v));
  _pos = mtx.getTranslation();

  setManipulatedNodeWorldMatrix(mtx);
}

bool ManipulatorInteractionHelper::hasManipulatedMode(RadixFeatures value)
{
  return (_manipulatedMode & value) != RadixFeatures::None;
}

bool ManipulatorInteractionHelper::hasManFlags(ManFlags value)
{
  return (_flags & value) != ManFlags::None;
}

ManFlags ManipulatorInteractionHelper::clearManFlags(ManFlags values)
{
  _flags &= ~values;
  return _flags;
}

ManFlags ManipulatorInteractionHelper::setManFlags(ManFlags values)
{
  _flags |= values;
  return _flags;
}

Vector3 ManipulatorInteractionHelper::ComputeRayHit(const Ray& ray,
                                                    float distance)
{
  return ray.origin.add(
    ray.direction.multiplyByFloats(distance, distance, distance));
}

bool ManipulatorInteractionHelper::instanceofAbstractMesh(Node* node)
{
  auto nodeType = node->type();
  return ((nodeType == IReflect::Type::ABSTRACTMESH)
          || (nodeType == IReflect::Type::GROUNDMESH)
          || (nodeType == IReflect::Type::INSTANCEDMESH)
          || (nodeType == IReflect::Type::LINESMESH)
          || (nodeType == IReflect::Type::MESH));
}

void ManipulatorInteractionHelper::setManipulatedNodeWorldMatrix(Matrix mtx)
{
  if (!_manipulatedNode) {
    return;
  }

  if (instanceofAbstractMesh(_manipulatedNode)) {
    auto mesh = static_cast<AbstractMesh*>(_manipulatedNode);

    if (mesh->parent()) {
      auto worldMatrix = *mesh->parent()->getWorldMatrix();
      mtx              = mtx.multiply(worldMatrix.invert());
    }

    auto pos   = Vector3::Zero();
    auto scale = Vector3::Zero();
    Quaternion rot;
    mtx.decompose(scale, rot, pos);
    mesh->setPosition(pos);
    mesh->setRotationQuaternion(rot);
    mesh->setScaling(scale);
  }
}

Matrix* ManipulatorInteractionHelper::getManipulatedNodeWorldMatrix()
{
  if (!_manipulatedNode) {
    return nullptr;
  }

  if (instanceofAbstractMesh(_manipulatedNode)) {
    return _manipulatedNode->getWorldMatrix();
  }

  return nullptr;
}

std::tuple<Plane, Vector3>
ManipulatorInteractionHelper::setupIntersectionPlane(RadixFeatures mode,
                                                     bool plane2)
{
  Plane p0, p1;
  std::tie(p0, p1) = setupIntersectionPlanes(mode);

  auto pl = plane2 ? p1 : p0;
  Vector3 axis;

  switch (mode) {
    case RadixFeatures::ArrowX:
      axis = _right;
      break;
    case RadixFeatures::ArrowY:
      axis = _up;
      break;
    case RadixFeatures::ArrowZ:
      axis = _view;
      break;
    case RadixFeatures::None:
    case RadixFeatures::ArrowsXYZ:
    case RadixFeatures::PlaneSelectionXY:
    case RadixFeatures::PlaneSelectionXZ:
    case RadixFeatures::PlaneSelectionYZ:
    case RadixFeatures::AllPlanesSelection:
    case RadixFeatures::RotationX:
    case RadixFeatures::RotationY:
    case RadixFeatures::RotationZ:
    case RadixFeatures::Rotations:
    case RadixFeatures::CenterSquare:
      axis = Vector3::Zero();
      break;
  }

  return std::make_tuple(pl, axis);
}

std::tuple<Plane, Plane>
ManipulatorInteractionHelper::setupIntersectionPlanes(RadixFeatures mode)
{
  Plane p0, p1;

  switch (mode) {
    case RadixFeatures::ArrowX:
      p0 = Plane::FromPoints(_pos, _pos.add(_view), _pos.add(_right));
      p1 = Plane::FromPoints(_pos, _pos.add(_right), _pos.add(_up));
      break;

    case RadixFeatures::ArrowY:
      p0 = Plane::FromPoints(_pos, _pos.add(_up), _pos.add(_right));
      p1 = Plane::FromPoints(_pos, _pos.add(_up), _pos.add(_view));
      break;

    case RadixFeatures::ArrowZ:
      p0 = Plane::FromPoints(_pos, _pos.add(_view), _pos.add(_right));
      p1 = Plane::FromPoints(_pos, _pos.add(_view), _pos.add(_up));
      break;

    case RadixFeatures::None:
    case RadixFeatures::ArrowsXYZ:
    case RadixFeatures::PlaneSelectionXY:
    case RadixFeatures::PlaneSelectionXZ:
    case RadixFeatures::PlaneSelectionYZ:
    case RadixFeatures::AllPlanesSelection:
    case RadixFeatures::RotationX:
    case RadixFeatures::RotationY:
    case RadixFeatures::RotationZ:
    case RadixFeatures::Rotations:
    case RadixFeatures::CenterSquare:
      break;
  }

  return std::make_tuple(p0, p1);
}

Vector2 ManipulatorInteractionHelper::getRayPosition(const PointerEvent& event)
{
  const auto canvasRect = _scene->getEngine()->getRenderingCanvasClientRect();

  const auto x = static_cast<float>(event.clientX - (*canvasRect).left);
  const auto y = static_cast<float>(event.clientY - (*canvasRect).top);

  return Vector2(x, y);
}

void ManipulatorInteractionHelper::renderManipulator()
{
  if (!_manipulatedNode) {
    return;
  }

  if (instanceofAbstractMesh(_manipulatedNode)) {
    auto mesh     = static_cast<AbstractMesh*>(_manipulatedNode);
    auto worldMtx = mesh->getWorldMatrix();
    auto l        = Vector3::Distance(_scene->activeCamera->position,
                               worldMtx->getTranslation());
    auto vpWidth  = _scene->getEngine()->getRenderWidth();
    auto s        = fromScreenToWorld(vpWidth / 100.f, l) * 20.f;
    auto scale    = Vector3::Zero();
    auto position = Vector3::Zero();
    auto rotation = Quaternion::Identity();

    auto res = Matrix::Scaling(s, s, s).multiply(*worldMtx);

    res.decompose(scale, rotation, position);

    _radix.setWorld(position, rotation, scale);
  }
}

float ManipulatorInteractionHelper::fromScreenToWorld(float l, float z)
{
  auto camera   = _scene->activeCamera;
  auto identity = std::make_unique<Matrix>(Matrix::Identity());
  auto r0       = _scene->createPickingRay(0, 0, identity.get(), camera, true);
  identity      = std::make_unique<Matrix>(Matrix::Identity());
  auto r1 = _scene->createPickingRay(static_cast<int>(l), 0, identity.get(),
                                     camera, true);

  auto p0 = ManipulatorInteractionHelper::evalPosition(*r0, z);
  auto p1 = ManipulatorInteractionHelper::evalPosition(*r1, z);

  return p1.x - p0.x;
}

Vector3 ManipulatorInteractionHelper::evalPosition(const Ray& ray, float u)
{
  return ray.origin.add(ray.direction.multiplyByFloats(u, u, u));
}

} // end of namespace Extensions
} // end of namespace BABYLON
