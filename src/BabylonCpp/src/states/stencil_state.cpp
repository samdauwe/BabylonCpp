#include <babylon/states/stencil_state.h>

#include <babylon/engines/engine.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

StencilState::StencilState()
    : stencilFunc{this, &StencilState::get_stencilFunc, &StencilState::set_stencilFunc}
    , stencilFuncRef{this, &StencilState::get_stencilFuncRef, &StencilState::set_stencilFuncRef}
    , stencilFuncMask{this, &StencilState::get_stencilFuncMask, &StencilState::set_stencilFuncMask}
    , stencilOpStencilFail{this, &StencilState::get_stencilOpStencilFail,
                           &StencilState::set_stencilOpStencilFail}
    , stencilOpDepthFail{this, &StencilState::get_stencilOpDepthFail,
                         &StencilState::set_stencilOpDepthFail}
    , stencilOpStencilDepthPass{this, &StencilState::get_stencilOpStencilDepthPass,
                                &StencilState::set_stencilOpStencilDepthPass}
    , stencilMask{this, &StencilState::get_stencilMask, &StencilState::set_stencilMask}
    , stencilTest{this, &StencilState::get_stencilTest, &StencilState::set_stencilTest}
{
}

StencilState::~StencilState() = default;

void StencilState::reset()
{
  enabled = false;
  mask    = 0xFF;

  func     = StencilState::ALWAYS;
  funcRef  = 1;
  funcMask = 0xFF;

  opStencilFail      = StencilState::KEEP;
  opDepthFail        = StencilState::KEEP;
  opStencilDepthPass = StencilState::REPLACE;
}

unsigned int StencilState::get_func() const
{
  return _func;
}

void StencilState::set_func(unsigned int value)
{
  _func = value;
}

unsigned int StencilState::get_funcRef() const
{
  return _funcRef;
}

void StencilState::set_funcRef(unsigned int value)
{
  _funcRef = value;
}

unsigned int StencilState::get_funcMask() const
{
  return _funcMask;
}

void StencilState::set_funcMask(unsigned int value)
{
  _funcMask = value;
}

unsigned int StencilState::get_opStencilFail() const
{
  return _opStencilFail;
}

void StencilState::set_opStencilFail(unsigned int value)
{
  _opStencilFail = value;
}

unsigned int StencilState::get_opDepthFail() const
{
  return _opDepthFail;
}

void StencilState::set_opDepthFail(unsigned int value)
{
  _opDepthFail = value;
}

unsigned int StencilState::get_opStencilDepthPass() const
{
  return _opStencilDepthPass;
}

void StencilState::set_opStencilDepthPass(unsigned int value)
{
  _opStencilDepthPass = value;
}

unsigned int StencilState::get_mask() const
{
  return _mask;
}

void StencilState::set_mask(unsigned int value)
{
  _mask = value;
}

bool StencilState::get_enabled() const
{
  return _enabled;
}

void StencilState::set_enabled(bool value)
{
  _enabled = value;
}

unsigned int StencilState::get_stencilFunc() const
{
  return _func;
}

void StencilState::set_stencilFunc(unsigned int value)
{
  _func = value;
}

int StencilState::get_stencilFuncRef() const
{
  return _funcRef;
}

void StencilState::set_stencilFuncRef(int value)
{
  _funcRef = value;
}

unsigned int StencilState::get_stencilFuncMask() const
{
  return _funcMask;
}

void StencilState::set_stencilFuncMask(unsigned int value)
{
  _funcMask = value;
}

unsigned int StencilState::get_stencilOpStencilFail() const
{
  return _opStencilFail;
}

void StencilState::set_stencilOpStencilFail(unsigned int value)
{
  _opStencilFail = value;
}

unsigned int StencilState::get_stencilOpDepthFail() const
{
  return _opDepthFail;
}

void StencilState::set_stencilOpDepthFail(unsigned int value)
{
  _opDepthFail = value;
}

unsigned int StencilState::get_stencilOpStencilDepthPass() const
{
  return _opStencilDepthPass;
}

void StencilState::set_stencilOpStencilDepthPass(unsigned int value)
{
  _opStencilDepthPass = value;
}

unsigned int StencilState::get_stencilMask() const
{
  return _mask;
}

void StencilState::set_stencilMask(unsigned int value)
{
  _mask = value;
}

bool StencilState::get_stencilTest() const
{
  return _enabled;
}

void StencilState::set_stencilTest(bool value)
{
  _enabled = value;
}

} // end of namespace BABYLON
