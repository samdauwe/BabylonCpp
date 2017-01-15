#include <babylon/extensions/hexplanetgeneration/terrain/plate.h>

#include <babylon/extensions/hexplanetgeneration/terrain/corner.h>
#include <babylon/extensions/hexplanetgeneration/utils/tools.h>

namespace BABYLON {
namespace Extensions {

Plate::Plate(const Color3& p_color, const Vector3& p_driftAxis,
             float p_driftRate, float p_spinRate, float p_elevation,
             bool p_oceanic, Corner& p_root)
    : color{p_color}
    , driftAxis{p_driftAxis}
    , driftRate{p_driftRate}
    , spinRate{p_spinRate}
    , elevation{p_elevation}
    , oceanic{p_oceanic}
    , root{&p_root}
    , tiles{}
    , boundaryCorners{}
    , boundaryBorders{}
    , area{0}
    , circumference{0}
{
}

Vector3 Plate::calculateMovement(const Vector3& position)
{
  auto movement = Tools::setLength(
    Vector3::Dot(driftAxis, position),
    driftRate * Vector3::Distance(Tools::projectOnVector(position, driftAxis),
                                  position));

  auto tmp = Tools::setLength(
    Vector3::Dot(root->position, position),
    spinRate * Vector3::Distance(
                 Tools::projectOnVector(position, root->position), position));

  movement += tmp;
  return movement;
}

} // end of namespace Extensions
} // end of namespace BABYLON
