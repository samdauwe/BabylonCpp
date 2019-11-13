#include <babylon/meshes/simplification/simplification_settings.h>

namespace BABYLON {

SimplificationSettings::SimplificationSettings(float _quality, float _distance,
                                               bool _optimizeMesh)
{
    quality = _quality;
    distance = _distance;
    optimizeMesh = _optimizeMesh;
}

SimplificationSettings::~SimplificationSettings() = default;

} // end of namespace BABYLON
