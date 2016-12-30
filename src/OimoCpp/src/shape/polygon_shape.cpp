#include <oimo/shape/polygon_shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/shape_config.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

PolygonShape::PolygonShape(const ShapeConfig& config,
                           const std::vector<Vertex>& verts,
                           const std::vector<Face>& faces)
    : Shape{config}, _vertices{verts}, _faces{faces}
{
  type = Shape::Type::SHAPE_POLYGON;
}

PolygonShape::~PolygonShape()
{
}

void PolygonShape::updateProxy()
{
  aabb->setFromPoints(_vertices);
  aabb->expandByScalar(AABB::AABB_PROX);

  if (proxy != nullptr) {
    proxy->update();
  }
}

} // end of namespace OIMO
