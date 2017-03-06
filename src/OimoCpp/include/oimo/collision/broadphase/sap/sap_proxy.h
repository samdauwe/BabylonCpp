#ifndef OIMO_COLLISION_BROADPHASE_SAP_SAP_PROXY_H
#define OIMO_COLLISION_BROADPHASE_SAP_SAP_PROXY_H

#include <vector>

#include <oimo/collision/broadphase/proxy.h>

namespace OIMO {

class SAPBroadPhase;
struct SAPElement;
class Shape;

/**
 * @brief A proxy for sweep and prune broad-phase.
 */
class SAPProxy : public Proxy {

public:
  SAPProxy(SAPBroadPhase* sap, Shape* shape);
  ~SAPProxy();

  /**
   * Returns whether the proxy is dynamic or not.
   */
  bool isDynamic() const;

  /**
   * Update the proxy.
   */
  void update() override;

public:
  // Type of the axis to which the proxy belongs to. [0:none, 1:dynamic,
  // 2:static]
  unsigned int belongsTo;
  // The maximum elements on each axis.
  std::vector<SAPElement> max;
  // The minimum elements on each axis.
  std::vector<SAPElement> min;
  SAPBroadPhase* sap;

}; // end of class SAPProxy

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_SAP_SAP_PROXY_H
