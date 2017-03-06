#include <oimo/collision/broadphase/broad_phase.h>

#include <oimo/collision/shape/shape.h>
#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_link.h>
#include <oimo/dynamics/rigid_body.h>

namespace OIMO {

BroadPhase::BroadPhase() : type{Type::BR_NULL}, numPairChecks{0}, numPairs{0}
{
}

BroadPhase::~BroadPhase()
{
}

bool BroadPhase::isAvailablePair(Shape* s1, Shape* s2)
{
  auto b1 = s1->parent;
  auto b2 = s2->parent;
  if (b1 == b2 ||                           // same parents
      (!b1->isDynamic && !b2->isDynamic) || // static or kinematic object
      (s1->belongsTo & s2->collidesWith) == 0
      || (s2->belongsTo & s1->collidesWith) == 0 // collision filtering
      ) {
    return false;
  }
  JointLink* js;
  if (b1->numJoints < b2->numJoints) {
    js = b1->jointLink;
  }
  else {
    js = b2->jointLink;
  }
  while (js != nullptr) {
    auto joint = js->joint;
    if (!joint->allowCollision
        && ((joint->body1 == b1 && joint->body2 == b2)
            || (joint->body1 == b2 && joint->body2 == b1))) {
      return false;
    }
    js = js->next;
  }

  return true;
}

void BroadPhase::detectPairs()
{
  // clear old
  pairs.clear();
  numPairs      = 0;
  numPairChecks = 0;

  collectPairs();
}

void BroadPhase::addPair(Shape* s1, Shape* s2)
{
  Pair pair{s1, s2};
  pairs.emplace_back(pair);
  ++numPairs;
}

} // end of namespace OIMO
