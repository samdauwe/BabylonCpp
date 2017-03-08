#ifndef OIMO_SHAPE_FACE_H
#define OIMO_SHAPE_FACE_H

#include <array>

#include <oimo/shape/vertex.h>

namespace OIMO {

/**
 * @brief Represents a face of a tetra.
 */
struct Face {

  Face();
  Face(unsigned int a, unsigned int b, unsigned int c);
  ~Face();

  unsigned int a, b, c;

}; // end of struct Face

} // end of namespace OIMO

#endif // end of OIMO_SHAPE_FACE_H
