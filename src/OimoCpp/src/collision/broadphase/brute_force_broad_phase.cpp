#include <oimo/collision/broadphase/brute_force_broad_phase.h>

#include <algorithm>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/basic_proxy.h>

namespace OIMO {

BruteForceBroadPhase::BruteForceBroadPhase()
    : BroadPhase{}, _type{BroadPhase::Type::BR_BRUTE_FORCE}
{
}

BruteForceBroadPhase::~BruteForceBroadPhase()
{
}

Proxy* BruteForceBroadPhase::createProxy(Shape* shape)
{
  return new BasicProxy(shape);
}

void BruteForceBroadPhase::addProxy(Proxy* proxy)
{
  _proxies.emplace_back(proxy);
}

void BruteForceBroadPhase::removeProxy(Proxy* proxy)
{
  _proxies.erase(std::remove_if(_proxies.begin(), _proxies.end(),
                                [proxy](const Proxy* p) { return p == proxy; }),
                 _proxies.end());
}

void BruteForceBroadPhase::collectPairs()
{
  Proxy *p1, *p2;
  size_t lu      = _proxies.size();
  int l          = static_cast<int>(lu);
  numPairChecks = l * (l - 1) >> 1;
  for (size_t i = 0; i < _proxies.size(); ++i) {
    p1 = _proxies[i];
    for (size_t j = i + 1; j < _proxies.size(); ++j) {
      p2 = _proxies[j];
      if (p1->aabb->intersectTest(*p2->aabb)
          || !isAvailablePair(p1->shape, p2->shape))
        continue;
      addPair(p1->shape, p2->shape);
    }
  }
}

BroadPhase::Type BruteForceBroadPhase::type() const
{
  return _type;
}

} // end of namespace OIMO
