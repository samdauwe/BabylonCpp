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
    : name{0}
    , prev{nullptr}
    , next{nullptr}
    , type{Type::BODY_NULL}
    , position{Vec3(x, y, z)}
    , orientation{rotationAxisToQuad(rad, ax, ay, az)}
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
  Shape* prevShape = _remove->prev;
  Shape* nextShape = _remove->next;
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
  std::array<float, 9>& te = localInertia.elements;
  Mat33 tmpM;
  Vec3 tmpV;
  float xy = 0.f, yz = 0.f, zx = 0.f;
  float shapeMass = 0.f, relX = 0.f, relY = 0.f, relZ = 0.f;
  for (Shape* shape = shapes; shape != nullptr; shape = shape->next) {
    shape->calculateMassInfo(massInfo);
    shapeMass = massInfo.mass;
    relX      = shape->relativePosition.x;
    relY      = shape->relativePosition.y;
    relZ      = shape->relativePosition.z;
    tmpV.addScale(shape->relativePosition, shapeMass);
    mass += shapeMass;
    rotateInertia(shape->relativeRotation, massInfo.inertia, tmpM);
    localInertia.addEqual(tmpM);
    // add offset inertia
    te[0] += shapeMass * (relY * relY + relZ * relZ);
    te[4] += shapeMass * (relX * relX + relZ * relZ);
    te[8] += shapeMass * (relX * relX + relY * relY);
    xy = shapeMass * relX * relY;
    yz = shapeMass * relY * relZ;
    zx = shapeMass * relZ * relX;
    te[1] -= xy;
    te[3] -= xy;
    te[2] -= yz;
    te[6] -= yz;
    te[5] -= zx;
    te[7] -= zx;
  }
  inverseMass = 1.f / mass;
  tmpV.scaleEqual(inverseMass);
  if (adjustPosition) {
    position.addEqual(tmpV);
    for (Shape* shape = shapes; shape != nullptr; shape = shape->next) {
      shape->relativePosition.subEqual(tmpV);
    }
    // subtract offset inertia
    relX = tmpV.x;
    relY = tmpV.y;
    relZ = tmpV.z;
    te[0] -= mass * (relY * relY + relZ * relZ);
    te[4] -= mass * (relX * relX + relZ * relZ);
    te[8] -= mass * (relX * relX + relY * relY);
    xy = mass * relX * relY;
    yz = mass * relY * relZ;
    zx = mass * relZ * relX;
    te[1] += xy;
    te[3] += xy;
    te[2] += yz;
    te[6] += yz;
    te[5] += zx;
    te[7] += zx;
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
  ContactLink* cs = contactLink;
  while (cs != nullptr) {
    cs->body->sleepTime = 0.f;
    cs->body->sleeping  = false;
    cs                  = cs->next;
  }
  JointLink* js = jointLink;
  while (js != nullptr) {
    js->body->sleepTime = 0.f;
    js->body->sleeping  = false;
    js                  = js->next;
  }
  for (Shape* shape = shapes; shape != nullptr; shape = shape->next) {
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
  for (Shape* shape = shapes; shape != nullptr; shape = shape->next) {
    shape->updateProxy();
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
      if (controlPos) {
        angularVelocity.set(0.f, 0.f, 0.f);
        linearVelocity.set(0.f, 0.f, 0.f);
        linearVelocity.x = (newPosition.x - position.x) / timeStep;
        linearVelocity.y = (newPosition.y - position.y) / timeStep;
        linearVelocity.z = (newPosition.z - position.z) / timeStep;
        controlPos       = false;
      }
      if (controlRot) {
        angularVelocity.set(0.f, 0.f, 0.f);
        orientation.copy(newOrientation);
        controlRot = false;
      }
      position.addTime(linearVelocity, timeStep);
      orientation.addTime(angularVelocity, timeStep);
      break;
    case Type::BODY_NULL:
    default:
      break;
  }

  syncShapes();
}

void RigidBody::rotateInertia(const Mat33& rot, const Mat33& inertia,
                              Mat33& out)
{
  const std::array<float, 9>& tm1 = rot.elements;
  const std::array<float, 9>& tm2 = inertia.elements;

  float a0 = tm1[0], a3 = tm1[3], a6 = tm1[6];
  float a1 = tm1[1], a4 = tm1[4], a7 = tm1[7];
  float a2 = tm1[2], a5 = tm1[5], a8 = tm1[8];

  float b0 = tm2[0], b3 = tm2[3], b6 = tm2[6];
  float b1 = tm2[1], b4 = tm2[4], b7 = tm2[7];
  float b2 = tm2[2], b5 = tm2[5], b8 = tm2[8];

  float e00 = a0 * b0 + a1 * b3 + a2 * b6;
  float e01 = a0 * b1 + a1 * b4 + a2 * b7;
  float e02 = a0 * b2 + a1 * b5 + a2 * b8;
  float e10 = a3 * b0 + a4 * b3 + a5 * b6;
  float e11 = a3 * b1 + a4 * b4 + a5 * b7;
  float e12 = a3 * b2 + a4 * b5 + a5 * b8;
  float e20 = a6 * b0 + a7 * b3 + a8 * b6;
  float e21 = a6 * b1 + a7 * b4 + a8 * b7;
  float e22 = a6 * b2 + a7 * b5 + a8 * b8;

  std::array<float, 9>& oe = out.elements;
  oe[0] = e00 * a0 + e01 * a1 + e02 * a2;
  oe[1] = e00 * a3 + e01 * a4 + e02 * a5;
  oe[2] = e00 * a6 + e01 * a7 + e02 * a8;
  oe[3] = e10 * a0 + e11 * a1 + e12 * a2;
  oe[4] = e10 * a3 + e11 * a4 + e12 * a5;
  oe[5] = e10 * a6 + e11 * a7 + e12 * a8;
  oe[6] = e20 * a0 + e21 * a1 + e22 * a2;
  oe[7] = e20 * a3 + e21 * a4 + e22 * a5;
  oe[8] = e20 * a6 + e21 * a7 + e22 * a8;
}

void RigidBody::syncShapes()
{
  float s  = orientation.s;
  float x  = orientation.x;
  float y  = orientation.y;
  float z  = orientation.z;
  float x2 = 2.f * x;
  float y2 = 2.f * y;
  float z2 = 2.f * z;
  float xx = x * x2;
  float yy = y * y2;
  float zz = z * z2;
  float xy = x * y2;
  float yz = y * z2;
  float xz = x * z2;
  float sx = s * x2;
  float sy = s * y2;
  float sz = s * z2;

  std::array<float, 9>& tr = rotation.elements;
  tr[0] = 1.f - yy - zz;
  tr[1] = xy - sz;
  tr[2] = xz + sy;
  tr[3] = xy + sz;
  tr[4] = 1 - xx - zz;
  tr[5] = yz - sx;
  tr[6] = xz - sy;
  tr[7] = yz + sx;
  tr[8] = 1.f - xx - yy;

  rotateInertia(rotation, inverseLocalInertia, inverseInertia);
  for (Shape* shape = shapes; shape != nullptr; shape = shape->next) {
    shape->position.mul(position, shape->relativePosition, rotation);
    shape->rotation.mul(rotation, shape->relativeRotation);
    shape->updateProxy();
  }
}

void RigidBody::applyImpulse(const Vec3& _position, const Vec3& force)
{
  linearVelocity.addScale(force, inverseMass);
  Vec3 rel;
  rel.sub(_position, _position).cross(rel, force).mulMat(inverseInertia, rel);
  angularVelocity.addEqual(rel);
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

void RigidBody::setPosition(const Vec3& pos)
{
  newPosition.copy(pos).multiplyScalar(World::INV_SCALE);
  controlPos = true;
}

void RigidBody::setQuaternion(const Quat& q)
{
  newOrientation.set(q.x, q.y, q.z, q.s);
  controlRot = true;
}

void RigidBody::setRotation(const Vec3& rot)
{
  newOrientation = rotationVectToQuad(rot);
  controlRot     = true;
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
  orientation = Quat(q.s, q.x, q.y, q.z);
  awake();
}

void RigidBody::resetRotation(float x, float y, float z)
{
  angularVelocity.set(0.f, 0.f, 0.f);
  orientation = rotationVectToQuad(Vec3(x, y, z));
  awake();
}

Vec3 RigidBody::getPosition()
{
  return Vec3().scale(position, World::WORLD_SCALE);
}

Euler RigidBody::getRotation()
{
  return Euler().setFromRotationMatrix(rotation);
}

Quat RigidBody::getQuaternion()
{
  return Quat().setFromRotationMatrix(rotation);
}

Mat44 RigidBody::getMatrix()
{
  std::array<float, 16>& m = matrix.elements;
  if (!sleeping) {
    // rotation matrix
    std::array<float, 9>& r = rotation.elements;
    m[0]  = r[0];
    m[1]  = r[3];
    m[2]  = r[6];
    m[3]  = 0.f;
    m[4]  = r[1];
    m[5]  = r[4];
    m[6]  = r[7];
    m[7]  = 0.f;
    m[8]  = r[2];
    m[9]  = r[5];
    m[10] = r[8];
    m[11] = 0.f;

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

  return matrix;
}

} // end of namespace OIMO
