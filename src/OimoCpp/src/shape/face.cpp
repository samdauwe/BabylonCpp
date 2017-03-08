#include <oimo/shape/face.h>

namespace OIMO {

Face::Face()
{
}

Face::Face(unsigned int _a, unsigned int _b, unsigned int _c)
    : a{_a}, b{_b}, c{_c}
{
}

Face::~Face()
{
}

} // end of namespace OIMO
