#include <oimo/collision/shape/shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/shape/shape_config.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

unsigned int Shape::s_nextID = 0;

Shape::Shape(const ShapeConfig& config)
    : type{Type::SHAPE_NULL}
    , id{s_nextID++}
    , prev{nullptr}
    , next{nullptr}
    , proxy{nullptr}
    , parent{nullptr}
    , contactLink{nullptr}
    , numContacts{0}
    , relativePosition{Vec3().copy(config.relativePosition)}
    , relativeRotation{Mat33().copy(config.relativeRotation)}
    , aabb{new AABB()}
    , density{config.density}
    , friction{config.friction}
    , restitution{config.restitution}
    , belongsTo{config.belongsTo}
    , collidesWith{config.collidesWith}
{
}

Shape::~Shape()
{
}

float Shape::volume() const
{
  return std::numeric_limits<float>::max();
}

void Shape::calculateMassInfo(MassInfo& /*out*/)
{
}

void Shape::updateProxy()
{
}

} // end of namespace OIMO
