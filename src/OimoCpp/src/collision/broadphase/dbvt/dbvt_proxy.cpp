#include <oimo/collision/broadphase/dbvt/dbvt_proxy.h>

#include <oimo/collision/broadphase/dbvt/dbvt_node.h>
#include <oimo/collision/shape/shape.h>

namespace OIMO {

DBVTProxy::DBVTProxy(Shape* _shape) : Proxy{_shape}
{
  leaf        = make_unique<DBVTNode>();
  leaf->proxy = this;
}

DBVTProxy::~DBVTProxy()
{
}

bool DBVTProxy::isDynamic() const
{
  return false;
}

void DBVTProxy::update()
{
}

} // end of namespace OIMO
