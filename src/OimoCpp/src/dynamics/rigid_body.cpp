#include <oimo/dynamics/rigid_body.h>

#include <cmath>

#include <oimo/collision/shape/shape.h>
#include <oimo/constraint/contact/contact_link.h>
#include <oimo/constraint/joint/joint_link.h>
#include <oimo/dynamics/world.h>
#include <oimo/math/axis.h>
#include <oimo/math/mat_rotation.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

RigidBody::RigidBody(float x, float y, float z, float rad, float ax, float ay,
                     float az)
    : name{""}
    , prev{nullptr}
    , next{nullptr}
    , type{Type::BODY_NULL}
    , position{Vec3(x, y, z)}
    , orientation{rotationAxisToQuad(rad, ax, ay, az)}
    , scale{1.f}
    , invScale{1.f}
    , controlPos{false}
    , controlRot{false}
    , controlRotInTime{false}
    , parent{nullptr}
    , contactLink{nullptr}
    , numContacts{0}
    , shapes{nullptr}
    , numShapes{0}
    , jointLink{nullptr}
    , numJoints{0}
    , isStatic{false}
    , isDynamic{false}
    , isKinematic{false}
    , mass{-1.f}
    , inverseMass{-1.f}
    , addedToIsland{false}
    , allowSleep{true}
    , sleepTime{0.f}
    , sleeping{false}
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::addShape(Shape* shape)
{
  if (shape->parent != nullptr) {
    // It is not possible that you add to the multi-rigid body the shape of one
    return;
  }
  if (shapes != nullptr) {
    shapes->prev       = shape;
    shapes->prev->next = shapes;
  }
  shapes        = shape;
  shape->parent = this;
  if (parent != nullptr) {
    parent->addShape(shape);
  }
  ++numShapes;
}

void RigidBody::removeShape(Shape* shape)
{
  Shape* _remove = shape;
  if (_remove->parent != this) {
    return;
  }
  auto prevShape = _remove->prev;
  auto nextShape = _remove->next;
  if (prevShape != nullptr) {
    prevShape->next = nextShape;
  }
  if (nextShape != nullptr) {
    nextShape->prev = prevShape;
  }
  if (shapes == _remove) {
    shapes = nextShape;
  }
  _remove->prev   = nullptr;
  _remove->next   = nullptr;
  _remove->parent = nullptr;
  if (parent != nullptr) {
    parent->removeShape(_remove);
  }
  --numShapes;
}

void RigidBody::remove()
{
  dispose();
}

void RigidBody::dispose()
{
  parent->removeRigidBody(this);
}

void RigidBody::checkContact(const std::string& _name)
{
  parent->checkContact(name, _name);
}

void RigidBody::setupMass(RigidBody::Type _type, bool adjustPosition)
{
  type      = _type;
  isDynamic = (type == Type::BODY_DYNAMIC);
  isStatic  = (type == Type::BODY_STATIC);

  mass = 0;
  localInertia.set(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);

  Mat33 tmpM;
  Vec3 tmpV;

  for (auto shape = shapes; shape != nullptr; shape = shape->next) {
    shape->calculateMassInfo(massInfo);
    auto shapeMass = massInfo.mass;
    tmpV.addScaledVector(shape->relativePosition, shapeMass);
    mass += shapeMass;
    rotateInertia(shape->relativeRotation, massInfo.inertia, tmpM);
    localInertia.add(tmpM);
    // add offset inertia
    localInertia.addOffset(shapeMass, shape->relativePosition);
  }

  inverseMass = 1.f / mass;
  tmpV.scaleEqual(inverseMass);

  if (adjustPosition) {
    position.addEqual(tmpV);
    for (auto shape = shapes; shape != nullptr; shape = shape->next) {
      shape->relativePosition.subEqual(tmpV);
    }
    // subtract offset inertia
    localInertia.subOffset(mass, tmpV);
  }

  inverseLocalInertia.invert(localInertia);

  if (type == Type::BODY_STATIC) {
    inverseMass = 0.f;
    inverseLocalInertia.set(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
  }

  syncShapes();
  awake();
}

void RigidBody::awake()
{
  if (!allowSleep || !sleeping) {
    return;
  }
  sleeping  = false;
  sleepTime = 0.f;
  // awake connected constraints
  auto cs = contactLink;
  while (cs != nullptr) {
    cs->body->sleepTime = 0.f;
    cs->body->sleeping  = false;
    cs                  = cs->next;
  }
  auto js = jointLink;
  while (js != nullptr) {
    js->body->sleepTime = 0.f;
    js->body->sleeping  = false;
    js                  = js->next;
  }
  for (auto shape = shapes; shape != nullptr; shape = shape->next) {
    shape->updateProxy();
  }
}

void RigidBody::sleep()
{
  if (!allowSleep || sleeping) {
    return;
  }
  linearVelocity.set(0.f, 0.f, 0.f);
  angularVelocity.set(0.f, 0.f, 0.f);
  sleepPosition.copy(position);
  sleepOrientation.copy(orientation);
  sleepTime = 0.f;
  sleeping  = true;
  for (auto shape = shapes; shape != nullptr; shape = shape->next) {
    shape->updateProxy();
  }
}

void RigidBody::testWakeUp()
{
  if (linearVelocity.testZero() || angularVelocity.testZero()
      || position.testDiff(sleepPosition)
      || orientation.testDiff(sleepOrientation)) {
    awake(); // awake the body}
  }
}

bool RigidBody::isLonely() const
{
  return numJoints == 0 && numContacts == 0;
}

void RigidBody::updatePosition(float timeStep)
{
  switch (type) {
    case Type::BODY_STATIC:
      linearVelocity.set(0.f, 0.f, 0.f);
      angularVelocity.set(0.f, 0.f, 0.f);
      // ONLY FOR TEST
      if (controlPos) {
        position.copy(newPosition);
        controlPos = false;
      }
      if (controlRot) {
        orientation.copy(newOrientation);
        controlRot = false;
      }
      break;
    case Type::BODY_DYNAMIC:
      if (isKinematic) {
        linearVelocity.set(0, 0, 0);
        angularVelocity.set(0, 0, 0);
      }
      if (controlPos) {
        linearVelocity.subVectors(newPosition, position)
          .multiplyScalar(1 / timeStep);
        controlPos = false;
      }
      if (controlRot) {
        angularVelocity.copy(getAxis());
        orientation.copy(newOrientation);
        controlRot = false;
      }
      position.addScaledVector(linearVelocity, timeStep);
      orientation.addTime(angularVelocity, timeStep);
      updateMesh();
      break;
    case Type::BODY_NULL:
    default:
      break;
  }

  syncShapes();
  updateMesh();
}

Vec3 RigidBody::getAxis()
{
  return Vec3(0.f, 1.f, 0.f)
    .applyMatrix3(inverseLocalInertia, true)
    .normalize();
}

void RigidBody::rotateInertia(const Mat33& rot, const Mat33& inertia,
                              Mat33& out)
{
  tmpInertia.multiplyMatrices(rot, inertia);
  out.multiplyMatrices(tmpInertia, rot, true);
}

Quat RigidBody::rotationVectToQuad(const Vec3& rot)
{
  Axis r = MatRotation::EulerToAxis(
    rot.x * Math::DEGTORAD, rot.y * Math::DEGTORAD, rot.z * Math::DEGTORAD);
  return rotationAxisToQuad(r.angle, r.x, r.y, r.z);
}

Quat RigidBody::rotationAxisToQuad(float rad, float ax, float ay, float az)
{
  // in radian
  float len = ax * ax + ay * ay + az * az;
  if (len > 0.f) {
    len = 1.f / std::sqrt(len);
    ax *= len;
    ay *= len;
    az *= len;
  }
  float _sin = std::sin(rad * 0.5f);
  float _cos = std::cos(rad * 0.5f);
  return Quat(_cos, _sin * ax, _sin * ay, _sin * az);
}

void RigidBody::syncShapes()
{
  rotation.setQuat(orientation);
  rotateInertia(rotation, inverseLocalInertia, inverseInertia);

  for (auto shape = shapes; shape != nullptr; shape = shape->next) {
    shape->position.copy(shape->relativePosition)
      .applyMatrix3(rotation, true)
      .add(position);
    // add by QuaziKb
    shape->rotation.multiplyMatrices(rotation, shape->relativeRotation);
    shape->updateProxy();
  }
}

void RigidBody::applyImpulse(const Vec3& _position, const Vec3& force)
{
  linearVelocity.addScaledVector(force, inverseMass);
  auto rel = Vec3().copy(_position).sub(position).cross(force).applyMatrix3(
    inverseInertia, true);
  angularVelocity.add(rel);
}

void RigidBody::setPosition(const Vec3& pos)
{
  newPosition.copy(pos).multiplyScalar(World::INV_SCALE);
  controlPos = true;
  if (!isKinematic) {
    isKinematic = true;
  }
}

void RigidBody::setQuaternion(const Quat& q)
{
  newOrientation.set(q.x, q.y, q.z, q.w);
  controlRot = true;
  if (!isKinematic) {
    isKinematic = true;
  }
}

void RigidBody::setRotation(const Vec3& rot)
{
  newOrientation = Quat().setFromEuler(
    rot.x * Math::DEGTORAD, rot.y * Math::DEGTORAD, rot.y * Math::DEGTORAD);
  controlRot = true;
}

void RigidBody::resetPosition(float x, float y, float z)
{
  linearVelocity.set(0.f, 0.f, 0.f);
  angularVelocity.set(0.f, 0.f, 0.f);
  position.set(x, y, z).multiplyScalar(World::INV_SCALE);
  awake();
}

void RigidBody::resetQuaternion(const Quat& q)
{
  angularVelocity.set(0.f, 0.f, 0.f);
  orientation = Quat(q.x, q.y, q.z, q.w);
  awake();
}

void RigidBody::resetRotation(float x, float y, float z)
{
  angularVelocity.set(0.f, 0.f, 0.f);
  orientation = Quat().setFromEuler(x * Math::DEGTORAD, y * Math::DEGTORAD,
                                    z * Math::DEGTORAD);
  awake();
}

Vec3 RigidBody::getPosition() const
{
  return pos;
}

Quat RigidBody::getQuaternion() const
{
  return quaternion;
}

std::array<float, 16> RigidBody::getMatrix()
{
  auto& m = matrix.elements;
  if (!sleeping) {
    // rotation matrix
    auto& r = rotation.elements;
    m[0]    = r[0];
    m[1]    = r[3];
    m[2]    = r[6];
    m[3]    = 0.f;
    m[4]    = r[1];
    m[5]    = r[4];
    m[6]    = r[7];
    m[7]    = 0.f;
    m[8]    = r[2];
    m[9]    = r[5];
    m[10]   = r[8];
    m[11]   = 0.f;

    // position matrix
    m[12] = position.x * World::WORLD_SCALE;
    m[13] = position.y * World::WORLD_SCALE;
    m[14] = position.z * World::WORLD_SCALE;

    // sleep or not ?
    m[15] = 0.f;
  }
  else {
    m[15] = 1.f;
  }

  return m;
}

void RigidBody::updateMesh()
{
  pos.scale(position, scale);
  quaternion.copy(orientation);

  meshPosition.copy(getPosition());
  meshQuaternion.copy(getQuaternion());
}

} // end of namespace OIMO
