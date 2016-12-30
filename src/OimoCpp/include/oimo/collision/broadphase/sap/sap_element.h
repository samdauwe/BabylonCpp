#ifndef OIMO_COLLISION_BROADPHASE_SAP_SAP_ELEMENT_H
#define OIMO_COLLISION_BROADPHASE_SAP_SAP_ELEMENT_H

namespace OIMO {

class SAPProxy;

/**
 * @brief An element of proxies.
 */
struct SAPElement {

  SAPElement(SAPProxy* proxy, bool max);
  ~SAPElement();

  // The parent proxy
  SAPProxy* proxy;
  // The pair element.
  SAPElement* pair;
  // The minimum element on other axis.
  SAPElement* min1;
  // The maximum element on other axis.
  SAPElement* max1;
  // The minimum element on other axis.
  SAPElement* min2;
  // The maximum element on other axis.
  SAPElement* max2;
  // Whether the element has maximum value or not.
  bool max;
  // The value of the element.
  float value;

}; // end of struct SAPElement

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_SAP_SAP_ELEMENT_H
