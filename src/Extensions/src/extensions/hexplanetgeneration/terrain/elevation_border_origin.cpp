#include <babylon/extensions/hexplanetgeneration/terrain/elevation_border_origin.h>

namespace BABYLON {
namespace Extensions {

ElevationBorderOrigin::ElevationBorderOrigin()
    : corner{nullptr}, pressure{0.f}, shear{0.f}, plate{nullptr}
{
}

ElevationBorderOrigin::ElevationBorderOrigin(
  Corner* p_corner, float p_pressure, float p_shear, Plate* p_plate,
  CalculateElevationFunc& p_calculateElevation)
    : corner{p_corner}
    , pressure{p_pressure}
    , shear{p_shear}
    , plate{p_plate}
    , calculateElevation{p_calculateElevation}
{
}

} // end of namespace Extensions
} // end of namespace BABYLON