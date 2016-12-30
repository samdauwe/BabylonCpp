#include <oimo/shape/vertex.h>

namespace OIMO {

Vertex::Vertex(float _x, float _y, float _z)
    : restLength{0.f}, stiffness{0.f}, x{_x}, y{_y}, z{_z}
{
}

Vertex::~Vertex()
{
}

} // end of namespace OIMO
