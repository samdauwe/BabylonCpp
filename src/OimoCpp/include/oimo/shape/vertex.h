#ifndef OIMO_SHAPE_VERTEX_H
#define OIMO_SHAPE_VERTEX_H

#include <vector>

namespace OIMO {

struct Face;

/**
 * @brief A polygon shape vertex, of point. Three of these make up a face for
 * the polygon.
 */
struct Vertex {

  Vertex(float x = 0, float y = 0, float z = 0);
  ~Vertex();

  // Soft body config
  float restLength;
  float stiffness;
  // Array of faces this vertex is connected to
  std::vector<Face*> uses;
  float x, y, z;

}; // end of struct Vertex

} // end of namespace OIMO

#endif // end of OIMO_SHAPE_VERTEX_H
