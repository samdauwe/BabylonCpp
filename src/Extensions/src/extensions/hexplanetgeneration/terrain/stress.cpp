#include <babylon/extensions/hexplanetgeneration/terrain/stress.h>

namespace BABYLON {
namespace Extensions {

Stress::Stress() : pressure{0.f}, shear{0.f}
{
}

Stress::Stress(float p_pressure, float p_shear)
    : pressure{p_pressure}, shear{p_shear}
{
}

} // end of namespace Extensions
} // end of namespace BABYLON