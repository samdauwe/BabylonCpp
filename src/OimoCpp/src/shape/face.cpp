#include <oimo/shape/face.h>

namespace OIMO {

Face::Face()
{
  a.uses.emplace_back(this);
  b.uses.emplace_back(this);
  c.uses.emplace_back(this);
}

Face::Face(const Vertex& _a, const Vertex& _b, const Vertex& _c)
    : a{_a}, b{_b}, c{_c}
{
  a.uses.emplace_back(this);
  b.uses.emplace_back(this);
  c.uses.emplace_back(this);
}

Face::~Face()
{
}

} // end of namespace OIMO
