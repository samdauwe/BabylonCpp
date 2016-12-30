#include <oimo/constraint/joint/joint.h>

#include <oimo/constraint/joint/joint_config.h>
#include <oimo/constraint/joint/joint_link.h>
#include <oimo/dynamics/rigid_body.h>
#include <oimo/dynamics/world.h>

namespace OIMO {

Joint::Joint(const JointConfig& config)
    : Constraint{}
    , name{""}
    , type{Type::JOINT_NULL}
    , prev{nullptr}
    , next{nullptr}
    , body1{config.body1}
    , body2{config.body2}
    , localAnchorPoint1{Vec3().copy(config.localAnchorPoint1)}
    , localAnchorPoint2{Vec3().copy(config.localAnchorPoint2)}
    , allowCollision{config.allowCollision}
    , b1Link{new JointLink(this)}
    , b2Link{new JointLink(this)}
{
}

Joint::~Joint()
{
}

void Joint::updateAnchorPoints()
{
  relativeAnchorPoint1.mulMat(body1->rotation, localAnchorPoint1);
  relativeAnchorPoint2.mulMat(body2->rotation, localAnchorPoint2);

  anchorPoint1.add(relativeAnchorPoint1, body1->position);
  anchorPoint2.add(relativeAnchorPoint2, body2->position);
}

void Joint::attach()
{
  b1Link->body = body2;
  b2Link->body = body1;
  if (body1->jointLink != nullptr) {
    b1Link->next       = body1->jointLink;
    b1Link->next->prev = b1Link;
  }
  else {
    b1Link->next = nullptr;
  }
  body1->jointLink = b1Link;
  ++body1->numJoints;
  if (body2->jointLink != nullptr) {
    b2Link->next       = body2->jointLink;
    b2Link->next->prev = b2Link;
  }
  else {
    b2Link->next = nullptr;
  }
  body2->jointLink = b2Link;
  ++body2->numJoints;
}

void Joint::detach()
{
  JointLink* _prev = b1Link->prev;
  JointLink* _next = b1Link->next;
  if (_prev != nullptr) {
    _prev->next = _next;
  }
  if (_next != nullptr) {
    _next->prev = _prev;
  }
  if (body1->jointLink == b1Link) {
    body1->jointLink = _next;
  }
  b1Link->prev = nullptr;
  b1Link->next = nullptr;
  b1Link->body = nullptr;
  --body1->numJoints;

  _prev = b2Link->prev;
  _next = b2Link->next;
  if (_prev != nullptr) {
    _prev->next = _next;
  }
  if (_next != nullptr) {
    _next->prev = _prev;
  }
  if (body2->jointLink == b2Link) {
    body2->jointLink = _next;
  }
  b2Link->prev = nullptr;
  b2Link->next = nullptr;
  b2Link->body = nullptr;
  --body2->numJoints;

  b1Link->body = nullptr;
  b2Link->body = nullptr;
}

void Joint::awake()
{
  body1->awake();
  body2->awake();
}

void Joint::dispose()
{
}

std::pair<Vec3, Vec3> Joint::getPosition() const
{
  Vec3 p1 = Vec3().scale(anchorPoint1, World::WORLD_SCALE);
  Vec3 p2 = Vec3().scale(anchorPoint2, World::WORLD_SCALE);
  return std::make_pair(p1, p2);
}

Mat33 Joint::getMatrix() const
{
  Mat33 mtx;
  std::array<float, 9>& m = mtx.elements;
  Vec3 p1 = anchorPoint1;
  Vec3 p2 = anchorPoint2;
  m[0]    = p1.x * World::WORLD_SCALE;
  m[1]    = p1.y * World::WORLD_SCALE;
  m[2]    = p1.z * World::WORLD_SCALE;
  m[3]    = 0.f;

  m[4] = p2.x * World::WORLD_SCALE;
  m[5] = p2.y * World::WORLD_SCALE;
  m[6] = p2.z * World::WORLD_SCALE;
  m[7] = 0.f;

  return mtx;
}

} // end of namespace OIMO
