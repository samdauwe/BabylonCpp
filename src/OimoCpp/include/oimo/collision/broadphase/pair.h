#ifndef OIMO_COLLISION_BROADPHASE_PAIR_H
#define OIMO_COLLISION_BROADPHASE_PAIR_H

namespace OIMO {

class Shape;

/**
 * @brief A pair of shapes that may collide.
 */
struct Pair {
  Pair();
  Pair(Shape* s1, Shape* s2);
  Pair(const Pair& p);
  Pair(Pair&& p);
  ~Pair();
  Pair& operator=(const Pair& p);
  Pair& operator=(Pair&& p);
  // The first shape.
  Shape* shape1;
  // The second shape.
  Shape* shape2;
}; // end of struct Pair

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_BROADPHASE_PAIR_H
