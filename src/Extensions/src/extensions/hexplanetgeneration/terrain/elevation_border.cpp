#include <babylon/extensions/hexplanetgeneration/terrain/elevation_border.h>

namespace BABYLON {
namespace Extensions {

ElevationBorder::ElevationBorder()
    : border{nullptr}
    , corner{nullptr}
    , nextCorner{nullptr}
    , distanceToPlateBoundary{0.f}
{
}

ElevationBorder::ElevationBorder(const ElevationBorderOrigin& p_origin,
                                 Border* p_border, Corner* p_corner,
                                 Corner* p_nextCorner,
                                 float p_distanceToPlateBoundary)
    : origin{p_origin}
    , border{p_border}
    , corner{p_corner}
    , nextCorner{p_nextCorner}
    , distanceToPlateBoundary{p_distanceToPlateBoundary}
{
}

} // end of namespace Extensions
} // end of namespace BABYLON
