#include <oimo/collision/broadphase/sap/sap_proxy.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/sap/sap_broad_phase.h>
#include <oimo/collision/broadphase/sap/sap_element.h>
#include <oimo/collision/shape/shape.h>
#include <oimo/dynamics/rigid_body.h>

namespace OIMO {

SAPProxy::SAPProxy(SAPBroadPhase* _sap, Shape* _shape)
    : Proxy{_shape}, belongsTo{0}, sap{_sap}
{
  min.emplace_back(new SAPElement(this, false));
  max.emplace_back(new SAPElement(this, true));
  min.emplace_back(new SAPElement(this, false));
  max.emplace_back(new SAPElement(this, true));
  min.emplace_back(new SAPElement(this, false));
  max.emplace_back(new SAPElement(this, true));
  max[0]->pair = min[0];
  max[1]->pair = min[1];
  max[2]->pair = min[2];
  min[0]->min1 = min[1];
  min[0]->max1 = max[1];
  min[0]->min2 = min[2];
  min[0]->max2 = max[2];
  min[1]->min1 = min[0];
  min[1]->max1 = max[0];
  min[1]->min2 = min[2];
  min[1]->max2 = max[2];
  min[2]->min1 = min[0];
  min[2]->max1 = max[0];
  min[2]->min2 = min[1];
  min[2]->max2 = max[1];
}

SAPProxy::~SAPProxy()
{
}

bool SAPProxy::isDynamic() const
{
  RigidBody* body = shape->parent;
  return body->isDynamic && !body->sleeping;
}

void SAPProxy::update()
{
  std::array<float, 6>& te = aabb->elements;
  min[0]->value = te[0];
  min[1]->value = te[1];
  min[2]->value = te[2];
  max[0]->value = te[3];
  max[1]->value = te[4];
  max[2]->value = te[5];

  if ((belongsTo == 1 && !isDynamic()) || (belongsTo == 2 && isDynamic())) {
    sap->removeProxy(this);
    sap->addProxy(this);
  }
}

} // end of namespace OIMO
