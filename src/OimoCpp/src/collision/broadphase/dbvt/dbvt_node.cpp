#include <oimo/collision/broadphase/dbvt/dbvt_node.h>

#include <oimo/collision/broadphase/aabb.h>

namespace OIMO {

DBVTNode::DBVTNode()
    : child1{nullptr}
    , child2{nullptr}
    , parent{nullptr}
    , proxy{nullptr}
    , height{0}
    , aabb{make_unique<AABB>()}
{
}

DBVTNode::~DBVTNode()
{
}

} // end of namespace OIMO