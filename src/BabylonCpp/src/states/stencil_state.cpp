#include <babylon/states/stencil_state.h>

#include <babylon/engines/engine.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

StencilState::StencilState(bool iReset)
    : isDirty{this, &StencilState::get_isDirty}
    , stencilFunc{this, &StencilState::get_stencilFunc, &StencilState::set_stencilFunc}
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
    , _isStencilTestDirty{false}
    , _isStencilMaskDirty{false}
    , _isStencilFuncDirty{false}
    , _isStencilOpDirty{false}
{
  if (iReset) {
    reset();
  }
}

StencilState::~StencilState() = default;

bool StencilState::get_isDirty() const
{
  return _isStencilTestDirty || _isStencilMaskDirty || _isStencilFuncDirty || _isStencilOpDirty;
}

unsigned int StencilState::get_stencilFunc() const
{
  return _stencilFunc;
}

void StencilState::set_stencilFunc(unsigned int value)
{
  if (_stencilFunc == value) {
    return;
  }

  _stencilFunc        = value;
  _isStencilFuncDirty = true;
}

int StencilState::get_stencilFuncRef() const
{
  return _stencilFuncRef;
}

void StencilState::set_stencilFuncRef(int value)
{
  if (_stencilFuncRef == value) {
    return;
  }

  _stencilFuncRef     = value;
  _isStencilFuncDirty = true;
}

unsigned int StencilState::get_stencilFuncMask() const
{
  return _stencilFuncMask;
}

void StencilState::set_stencilFuncMask(unsigned int value)
{
  if (_stencilFuncMask == value) {
    return;
  }

  _stencilFuncMask    = value;
  _isStencilFuncDirty = true;
}

unsigned int StencilState::get_stencilOpStencilFail() const
{
  return _stencilOpStencilFail;
}

void StencilState::set_stencilOpStencilFail(unsigned int value)
{
  if (_stencilOpStencilFail == value) {
    return;
  }

  _stencilOpStencilFail = value;
  _isStencilOpDirty     = true;
}

unsigned int StencilState::get_stencilOpDepthFail() const
{
  return _stencilOpDepthFail;
}

void StencilState::set_stencilOpDepthFail(unsigned int value)
{
  if (_stencilOpDepthFail == value) {
    return;
  }

  _stencilOpDepthFail = value;
  _isStencilOpDirty   = true;
}

unsigned int StencilState::get_stencilOpStencilDepthPass() const
{
  return _stencilOpStencilDepthPass;
}

void StencilState::set_stencilOpStencilDepthPass(unsigned int value)
{
  if (_stencilOpStencilDepthPass == value) {
    return;
  }

  _stencilOpStencilDepthPass = value;
  _isStencilOpDirty          = true;
}

unsigned int StencilState::get_stencilMask() const
{
  return _stencilMask;
}

void StencilState::set_stencilMask(unsigned int value)
{
  if (_stencilMask == value) {
    return;
  }

  _stencilMask        = value;
  _isStencilMaskDirty = true;
}

bool StencilState::get_stencilTest() const
{
  return _stencilTest;
}

void StencilState::set_stencilTest(bool value)
{
  if (_stencilTest == value) {
    return;
  }

  _stencilTest        = value;
  _isStencilTestDirty = true;
}

void StencilState::reset()
{
  _stencilTest = false;
  _stencilMask = 0xFF;

  _stencilFunc     = StencilState::ALWAYS;
  _stencilFuncRef  = 1;
  _stencilFuncMask = 0xFF;

  _stencilOpStencilFail      = StencilState::KEEP;
  _stencilOpDepthFail        = StencilState::KEEP;
  _stencilOpStencilDepthPass = StencilState::REPLACE;

  _isStencilTestDirty = true;
  _isStencilMaskDirty = true;
  _isStencilFuncDirty = true;
  _isStencilOpDirty   = true;
}

void StencilState::apply(GL::IGLRenderingContext& gl)
{
  if (!isDirty()) {
    return;
  }

  // Stencil test
  if (_isStencilTestDirty) {
    if (stencilTest()) {
      gl.enable(GL::STENCIL_TEST);
    }
    else {
      gl.disable(GL::STENCIL_TEST);
    }
    _isStencilTestDirty = false;
  }

  // Stencil mask
  if (_isStencilMaskDirty) {
    gl.stencilMask(stencilMask());
    _isStencilMaskDirty = false;
  }

  // Stencil func
  if (_isStencilFuncDirty) {
    gl.stencilFunc(stencilFunc(), stencilFuncRef(), stencilFuncMask());
    _isStencilFuncDirty = false;
  }

  // Stencil op
  if (_isStencilOpDirty) {
    gl.stencilOp(stencilOpStencilFail(), stencilOpDepthFail(), stencilOpStencilDepthPass());
    _isStencilOpDirty = false;
  }
}

} // end of namespace BABYLON
