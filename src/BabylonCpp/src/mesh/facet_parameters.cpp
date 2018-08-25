#include <babylon/mesh/facet_parameters.h>

namespace BABYLON {

FacetParameters::FacetParameters()
    : subDiv{1, 1, 1, 1}
    , ratio{1.f}
    , bInfo{Vector3::Zero(), Vector3::Zero()}
    , useRightHandedSystem{false}
    , depthSort{false}
    , distanceTo{nullopt_t}
{
}

FacetParameters::~FacetParameters()
{
}

} // end of namespace BABYLON
