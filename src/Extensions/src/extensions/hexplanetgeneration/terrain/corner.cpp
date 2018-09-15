#include <babylon/extensions/hexplanetgeneration/terrain/corner.h>

#include <sstream>

#include <babylon/extensions/hexplanetgeneration/terrain/border.h>
#include <babylon/extensions/hexplanetgeneration/terrain/tile.h>

namespace BABYLON {
namespace Extensions {

Corner::Corner(size_t p_id, const Vector3& p_position, size_t cornerCount,
               size_t borderCount, size_t tileCount)
    : UidObj{p_id}
    , position{p_position}
    , area{0}
    , corners{}
    , borders{}
    , tiles{}
    , distanceToPlateRoot{0}
    , distanceToPlateBoundary{0}
    , betweenPlates(false)
    , pressure{0}
    , shear{0}
    , elevation{0}
    , airCurrent()
    , airCurrentSpeed{0}
    , airCurrentOutflows()
    , temperature{0}
    , airHeat{0}
    , newAirHeat{0}
    , heat{0}
    , heatAbsorption{0}
    , maxHeat{0}
    , moisture{0}
    , airMoisture{0}
    , newAirMoisture{0}
    , precipitation{0}
    , precipitationRate{0}
    , maxPrecipitation{0}
{
  corners.resize(cornerCount);
  borders.resize(borderCount);
  tiles.resize(tileCount);
}

Vector3 Corner::vectorTo(const Corner& corner)
{
  return corner.position - position;
}

std::string Corner::toString()
{
  std::ostringstream oss;
  oss << "Corner " << id << " < " << position.x << ", " << position.y << ", "
      << position.z << " >";
  return oss.str();
}

} // end of namespace Extensions
} // end of namespace BABYLON
