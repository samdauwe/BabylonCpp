#include <oimo/constraint/contact/contact.h>

#include <cmath>

#include <oimo/collision/narrowphase/collision_detector.h>
#include <oimo/collision/shape/shape.h>
#include <oimo/constraint/contact/contact_constraint.h>
#include <oimo/constraint/contact/contact_link.h>
#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/dynamics/rigid_body.h>

namespace OIMO {

Contact::Contact()
    : shape1{nullptr}
    , shape2{nullptr}
    , body1{nullptr}
    , body2{nullptr}
    , prev{nullptr}
    , next{nullptr}
    , persisting{false}
    , sleeping{false}
    , detector{nullptr}
    , touching{false}
    , close{false}
    , dist{std::numeric_limits<float>::infinity()}
    , b1Link{make_unique<ContactLink>(this)}
    , b2Link{make_unique<ContactLink>(this)}
    , s1Link{make_unique<ContactLink>(this)}
    , s2Link{make_unique<ContactLink>(this)}
    , manifold{make_unique<ContactManifold>()}
    , constraint{make_unique<ContactConstraint>(manifold.get())}
{
  points = manifold->points;
}

Contact::~Contact()
{
}

float Contact::MixRestitution(float restitution1, float restitution2)
{
  return std::sqrt(restitution1 * restitution2);
}

float Contact::MixFriction(float friction1, float friction2)
{
  return std::sqrt(friction1 * friction2);
}

void Contact::updateManifold()
{
  constraint->restitution
    = Contact::MixRestitution(shape1->restitution, shape2->restitution);
  constraint->friction
    = Contact::MixFriction(shape1->friction, shape2->friction);
  auto numBuffers = manifold->numPoints;
  for (unsigned int i = numBuffers; i-- > 0;) {
    auto& b   = buffer[i];
    auto& p   = points[i];
    b.lp1X    = p.localPoint1.x;
    b.lp1Y    = p.localPoint1.y;
    b.lp1Z    = p.localPoint1.z;
    b.lp2X    = p.localPoint2.x;
    b.lp2Y    = p.localPoint2.y;
    b.lp2Z    = p.localPoint2.z;
    b.impulse = p.normalImpulse;
  }
  manifold->numPoints = 0;
  detector->detectCollision(shape1, shape2, manifold.get());
  auto num = manifold->numPoints;
  if (num == 0) {
    touching = false;
    close    = false;
    dist     = std::numeric_limits<float>::infinity();
    return;
  }

  if (touching || dist < 0.001f) {
    close = true;
  }
  touching = true;
  for (unsigned int i = num; i-- > 0;) {
    auto& p            = points[i];
    float lp1x         = p.localPoint1.x;
    float lp1y         = p.localPoint1.y;
    float lp1z         = p.localPoint1.z;
    float lp2x         = p.localPoint2.x;
    float lp2y         = p.localPoint2.y;
    float lp2z         = p.localPoint2.z;
    bool indexSet      = false;
    unsigned int index = 0;
    float minDistance  = 0.0004f;
    for (unsigned int j = numBuffers; j-- > 0;) {
      ImpulseDataBuffer& b = buffer[j];
      float dx             = b.lp1X - lp1x;
      float dy             = b.lp1Y - lp1y;
      float dz             = b.lp1Z - lp1z;
      float distance1      = dx * dx + dy * dy + dz * dz;
      dx                   = b.lp2X - lp2x;
      dy                   = b.lp2Y - lp2y;
      dz                   = b.lp2Z - lp2z;
      float distance2      = dx * dx + dy * dy + dz * dz;
      if (distance1 < distance2) {
        if (distance1 < minDistance) {
          minDistance = distance1;
          index       = j;
          indexSet    = true;
        }
      }
      else {
        if (distance2 < minDistance) {
          minDistance = distance2;
          index       = j;
          indexSet    = true;
        }
      }
    }
    if (indexSet) {
      auto& tmp          = buffer[index];
      buffer[index]      = buffer[--numBuffers];
      buffer[numBuffers] = tmp;
      p.normalImpulse    = tmp.impulse;
      p.warmStarted      = true;
    }
    else {
      p.normalImpulse = 0;
      p.warmStarted   = false;
    }
  }
}

void Contact::attach(Shape* _shape1, Shape* _shape2)
{
  shape1 = _shape1;
  shape2 = _shape2;
  body1  = shape1->parent;
  body2  = shape2->parent;

  manifold->body1   = body1;
  manifold->body2   = body2;
  constraint->body1 = body1;
  constraint->body2 = body2;
  constraint->attach();

  s1Link->shape = _shape2;
  s1Link->body  = body2;
  s2Link->shape = _shape1;
  s2Link->body  = body1;

  if (shape1->contactLink != nullptr) {
    s1Link->next       = _shape1->contactLink;
    s1Link->next->prev = s1Link.get();
  }
  else {
    s1Link->next = nullptr;
  }
  _shape1->contactLink = s1Link.get();
  ++_shape1->numContacts;

  if (shape2->contactLink != nullptr) {
    s2Link->next       = _shape2->contactLink;
    s2Link->next->prev = s2Link.get();
  }
  else {
    s2Link->next = nullptr;
  }
  _shape2->contactLink = s2Link.get();
  ++_shape2->numContacts;

  b1Link->shape = _shape2;
  b1Link->body  = body2;
  b2Link->shape = _shape1;
  b2Link->body  = body1;

  if (body1->contactLink != nullptr) {
    b1Link->next       = body1->contactLink;
    b1Link->next->prev = b1Link.get();
  }
  else {
    b1Link->next = nullptr;
  }
  body1->contactLink = b1Link.get();
  ++body1->numContacts;

  if (body2->contactLink != nullptr) {
    b2Link->next       = body2->contactLink;
    b2Link->next->prev = b2Link.get();
  }
  else {
    b2Link->next = nullptr;
  }
  body2->contactLink = b2Link.get();
  ++body2->numContacts;

  prev = nullptr;
  next = nullptr;

  persisting          = true;
  sleeping            = body1->sleeping && body2->sleeping;
  manifold->numPoints = 0;
}

void Contact::detach()
{
  auto _prev = s1Link->prev;
  auto _next = s1Link->next;
  if (_prev != nullptr) {
    _prev->next = _next;
  }
  if (_next != nullptr) {
    _next->prev = _prev;
  }
  if (shape1->contactLink == s1Link.get()) {
    shape1->contactLink = _next;
  }
  s1Link->prev  = nullptr;
  s1Link->next  = nullptr;
  s1Link->shape = nullptr;
  s1Link->body  = nullptr;
  --shape1->numContacts;

  _prev = s2Link->prev;
  _next = s2Link->next;
  if (_prev != nullptr) {
    _prev->next = _next;
  }
  if (_next != nullptr) {
    _next->prev = _prev;
  }
  if (shape2->contactLink == s2Link.get()) {
    shape2->contactLink = _next;
  }
  s2Link->prev  = nullptr;
  s2Link->next  = nullptr;
  s2Link->shape = nullptr;
  s2Link->body  = nullptr;
  --shape2->numContacts;

  _prev = b1Link->prev;
  _next = b1Link->next;
  if (_prev != nullptr) {
    _prev->next = _next;
  }
  if (_next != nullptr) {
    _next->prev = _prev;
  }
  if (body1->contactLink == b1Link.get()) {
    body1->contactLink = _next;
  }
  b1Link->prev  = nullptr;
  b1Link->next  = nullptr;
  b1Link->shape = nullptr;
  b1Link->body  = nullptr;
  --body1->numContacts;

  _prev = b2Link->prev;
  _next = b2Link->next;
  if (_prev != nullptr) {
    _prev->next = _next;
  }
  if (_next != nullptr) {
    _next->prev = _prev;
  }
  if (body2->contactLink == b2Link.get()) {
    body2->contactLink = _next;
  }
  b2Link->prev  = nullptr;
  b2Link->next  = nullptr;
  b2Link->shape = nullptr;
  b2Link->body  = nullptr;
  --body2->numContacts;

  manifold->body1   = nullptr;
  manifold->body2   = nullptr;
  constraint->body1 = nullptr;
  constraint->body2 = nullptr;
  constraint->detach();

  shape1 = nullptr;
  shape2 = nullptr;
  body1  = nullptr;
  body2  = nullptr;
}

} // end of namespace OIMO
