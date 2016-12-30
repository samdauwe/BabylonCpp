#include <oimo/collision/broadphase/basic_proxy.h>

#include <oimo/collision/shape/shape.h>

namespace OIMO {

BasicProxy::BasicProxy(Shape* _shape) : Proxy{_shape}
{
  id = s_proxyID++;
}

BasicProxy::~BasicProxy()
{
}

void BasicProxy::update()
{
}

} // end of namespace OIMO
