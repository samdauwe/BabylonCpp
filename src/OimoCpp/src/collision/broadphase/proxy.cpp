#include <oimo/collision/broadphase/proxy.h>

#include <oimo/collision/shape/shape.h>

namespace OIMO {

unsigned int Proxy::s_proxyID = 0;

Proxy::Proxy(Shape* _shape) : shape{_shape}, aabb{_shape->aabb}
{
}

Proxy::~Proxy()
{
}

} // end of namespace OIMO
