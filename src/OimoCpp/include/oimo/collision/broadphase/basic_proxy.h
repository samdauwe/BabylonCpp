#ifndef OIMO_COLLISION_BROADPHASE_BASIC_PROXY_H
#define OIMO_COLLISION_BROADPHASE_BASIC_PROXY_H

#include <oimo/collision/broadphase/proxy.h>

namespace OIMO {

class Shape;

/**
 * @brief A basic implementation of proxies.
 */
class BasicProxy : public Proxy {

public:
  BasicProxy(Shape* shape);
  virtual ~BasicProxy();

  /**
   * Update the proxy.
   */
  virtual void update() override;

public:
  unsigned int id;

}; // end of class Proxy

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_BASIC_PROXY_H
