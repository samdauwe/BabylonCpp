#include <babylon/extensions/hexplanetgeneration/terrain/whorl.h>

namespace BABYLON {
namespace Extensions {

Whorl::Whorl() : center{}, strength{0.f}, radius{0.f}
{
}

Whorl::Whorl(Vector3&& p_center, float p_strength, float p_radius)
    : center{p_center}, strength{p_strength}, radius{p_radius}
{
}

} // end of namespace Extensions
} // end of namespace BABYLON
