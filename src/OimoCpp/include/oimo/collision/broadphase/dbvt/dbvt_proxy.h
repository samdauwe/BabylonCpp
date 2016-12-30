#ifndef OIMO_COLLISION_BROADPHASE_DBVT_DBVT_PROXY_H
#define OIMO_COLLISION_BROADPHASE_DBVT_DBVT_PROXY_H

#include <oimo/collision/broadphase/proxy.h>

namespace OIMO {

struct DBVTNode;
class Shape;

/**
 * @brief A proxy for dynamic bounding volume tree broad-phase.
 */
class DBVTProxy : public Proxy {

public:
  DBVTProxy(Shape* shape);
  ~DBVTProxy();

  /**
   * Returns whether the proxy is dynamic or not.
   */
  bool isDynamic() const;

  /**
   * Update the proxy.
   */
  void update() override;

public:
  // The leaf of the proxy.
  DBVTNode* leaf;

}; // end of class DBVTProxy

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_DBVT_DBVT_PROXY_H
