#ifndef OIMO_COLLISION_BROADPHASE_PROXY_H
#define OIMO_COLLISION_BROADPHASE_PROXY_H

namespace OIMO {

class AABB;
class Shape;

/**
 * @brief A proxy is used for broad-phase collecting pairs that can be
 * colliding.
 */
class Proxy {

public:
  static unsigned int s_proxyID;

public:
  Proxy(Shape* shape);
  virtual ~Proxy();

  /**
   * Update the proxy.
   */
  virtual void update() = 0;

public:
  // The parent shape.
  Shape* shape;
  // The axis-aligned bounding box.
  AABB* aabb;

}; // end of class Proxy

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_PROXY_H
