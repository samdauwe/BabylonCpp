#include <oimo/collision/broadphase/sap/sap_broad_phase.h>

#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/broadphase/sap/sap_axis.h>
#include <oimo/collision/broadphase/sap/sap_element.h>
#include <oimo/collision/broadphase/sap/sap_proxy.h>
#include <oimo/collision/shape/shape.h>

namespace OIMO {

SAPBroadPhase::SAPBroadPhase()
    : BroadPhase{}, _numElementsD{0}, _numElementsS{0}, _index1{0}, _index2{1}
{
  type = BroadPhase::Type::BR_SWEEP_AND_PRUNE;
  // dynamic proxies
  _axesD = {new SAPAxis(), //
            new SAPAxis(), //
            new SAPAxis()};
  // static or sleeping proxies
  _axesS = {new SAPAxis(), //
            new SAPAxis(), //
            new SAPAxis()};
}

SAPBroadPhase::~SAPBroadPhase()
{
}

Proxy* SAPBroadPhase::createProxy(Shape* shape)
{
  return new SAPProxy(this, shape);
}

void SAPBroadPhase::addProxy(Proxy* proxy)
{
  SAPProxy* p = dynamic_cast<SAPProxy*>(proxy);
  if (p == nullptr) {
    return;
  }
  if (p->isDynamic()) {
    _axesD[0]->addElements(p->min[0], p->max[0]);
    _axesD[1]->addElements(p->min[1], p->max[1]);
    _axesD[2]->addElements(p->min[2], p->max[2]);
    p->belongsTo = 1;
    _numElementsD += 2;
  }
  else {
    _axesS[0]->addElements(p->min[0], p->max[0]);
    _axesS[1]->addElements(p->min[1], p->max[1]);
    _axesS[2]->addElements(p->min[2], p->max[2]);
    p->belongsTo = 2;
    _numElementsS += 2;
  }
}

void SAPBroadPhase::removeProxy(Proxy* proxy)
{
  SAPProxy* p = dynamic_cast<SAPProxy*>(proxy);
  if (p == nullptr) {
    return;
  }
  if (p->belongsTo == 0) {
    return;
  }

  switch (p->belongsTo) {
    case 1:
      _axesD[0]->removeElements(p->min[0], p->max[0]);
      _axesD[1]->removeElements(p->min[1], p->max[1]);
      _axesD[2]->removeElements(p->min[2], p->max[2]);
      _numElementsD -= 2;
      break;
    case 2:
      _axesS[0]->removeElements(p->min[0], p->max[0]);
      _axesS[1]->removeElements(p->min[1], p->max[1]);
      _axesS[2]->removeElements(p->min[2], p->max[2]);
      _numElementsS -= 2;
      break;
    default:
      break;
  }

  p->belongsTo = 0;
}

void SAPBroadPhase::collectPairs()
{
  if (_numElementsD == 0) {
    return;
  }

  SAPAxis* axis1 = _axesD[_index1];
  SAPAxis* axis2 = _axesD[_index2];

  axis1->sort();
  axis2->sort();

  int count1 = axis1->calculateTestCount();
  int count2 = axis2->calculateTestCount();
  std::vector<SAPElement*> elementsD;
  std::vector<SAPElement*> elementsS;
  if (count1 <= count2) { // select the best axis
    axis2 = _axesS[_index1];
    axis2->sort();
    elementsD = axis1->elements;
    elementsS = axis2->elements;
  }
  else {
    axis1 = _axesS[_index2];
    axis1->sort();
    elementsD = axis2->elements;
    elementsS = axis1->elements;
    _index1 ^= _index2;
    _index2 ^= _index1;
    _index1 ^= _index2;
  }
  unsigned int p = 0, q = 0;
  SAPElement *activeD = nullptr, *activeS = nullptr;
  SAPElement *s = nullptr, *d = nullptr;
  SAPElement *e1 = nullptr, *e2 = nullptr, *min = nullptr;
  Shape *s1 = nullptr, *s2 = nullptr;
  float min1 = 0.f, max1 = 0.f, min2 = 0.f, max2 = 0.f;
  bool dyn = false;
  while (p < _numElementsD) {
    if (q == _numElementsS) {
      e1  = elementsD[p];
      dyn = true;
      ++p;
    }
    else {
      d = elementsD[p];
      s = elementsS[q];
      if (d->value < s->value) {
        e1  = d;
        dyn = true;
        ++p;
      }
      else {
        e1  = s;
        dyn = false;
        ++q;
      }
    }
    if (!e1->max) {
      s1   = e1->proxy->shape;
      min1 = e1->min1->value;
      max1 = e1->max1->value;
      min2 = e1->min2->value;
      max2 = e1->max2->value;

      for (SAPElement* _e2 = activeD; _e2 != nullptr;
           _e2             = _e2->pair) { // test for dynamic
        s2 = _e2->proxy->shape;

        ++numPairChecks;
        if (min1 > _e2->max1->value || max1 < _e2->min1->value
            || min2 > _e2->max2->value || max2 < _e2->min2->value
            || !isAvailablePair(s1, s2))
          continue;
        addPair(s1, s2);
      }
      if (dyn) {
        for (e2 = activeS; e2 != nullptr; e2 = e2->pair) { // test for static
          s2 = e2->proxy->shape;

          ++numPairChecks;

          if (min1 > e2->max1->value || max1 < e2->min1->value
              || min2 > e2->max2->value || max2 < e2->min2->value
              || !isAvailablePair(s1, s2))
            continue;
          addPair(s1, s2);
        }
        e1->pair = activeD;
        activeD  = e1;
      }
      else {
        e1->pair = activeS;
        activeS  = e1;
      }
    }
    else {
      min = e1->pair;
      if (dyn) {
        if (min == activeD) {
          activeD = activeD->pair;
          continue;
        }
        else {
          e1 = activeD;
        }
      }
      else {
        if (min == activeS) {
          activeS = activeS->pair;
          continue;
        }
        else {
          e1 = activeS;
        }
      }
      do {
        e2 = e1->pair;
        if (e2 == min) {
          e1->pair = e2->pair;
          break;
        }
        e1 = e2;
      } while (e1 != nullptr);
    }
  }
  _index2 = (_index1 | _index2) ^ 3;
}

} // end of namespace OIMO
