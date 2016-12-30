#ifndef OIMO_SHAPE_FACE_H
#define OIMO_SHAPE_FACE_H

#include <array>

#include <oimo/shape/vertex.h>

namespace OIMO {

/**
 * @brief A polygon shape face, made from 3 vertices.
 */
struct Face {

  Face();
  Face(const Vertex& a, const Vertex& b, const Vertex& c);
  ~Face();

  Vertex a, b, c;

}; // end of struct Face

} // end of namespace OIMO

#endif // end of OIMO_SHAPE_FACE_H
