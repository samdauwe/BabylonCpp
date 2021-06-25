#include <babylon/states/istencil_state.h>

namespace BABYLON {

IStencilState::IStencilState()
    : enabled{this, &IStencilState::get_enabled, &IStencilState::set_enabled}
    , mask{this, &IStencilState::get_mask, &IStencilState::set_mask}
    , func{this, &IStencilState::get_func, &IStencilState::set_func}
    , funcRef{this, &IStencilState::get_funcRef, &IStencilState::set_funcRef}
    , funcMask{this, &IStencilState::get_funcMask, &IStencilState::set_funcMask}
    , opStencilDepthPass{this, &IStencilState::get_opStencilDepthPass,
                         &IStencilState::set_opStencilDepthPass}
    , opStencilFail{this, &IStencilState::get_opStencilFail, &IStencilState::set_opStencilFail}
    , opDepthFail{this, &IStencilState::get_opDepthFail, &IStencilState::set_opDepthFail}
{
}

IStencilState::~IStencilState() = default;

} // end of namespace BABYLON
