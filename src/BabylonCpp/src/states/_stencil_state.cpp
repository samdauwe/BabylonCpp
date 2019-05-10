#include <babylon/states/_stencil_state.h>

#include <babylon/engine/engine.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

_StencilState::_StencilState()
    : isDirty{this, &_StencilState::get_isDirty}
    , stencilFunc{this, &_StencilState::get_stencilFunc,
                  &_StencilState::set_stencilFunc}
    , stencilFuncRef{this, &_StencilState::get_stencilFuncRef,
                     &_StencilState::set_stencilFuncRef}
    , stencilFuncMask{this, &_StencilState::get_stencilFuncMask,
                      &_StencilState::set_stencilFuncMask}
    , stencilOpStencilFail{this, &_StencilState::get_stencilOpStencilFail,
                           &_StencilState::set_stencilOpStencilFail}
    , stencilOpDepthFail{this, &_StencilState::get_stencilOpDepthFail,
                         &_StencilState::set_stencilOpDepthFail}
    , stencilOpStencilDepthPass{this,
                                &_StencilState::get_stencilOpStencilDepthPass,
                                &_StencilState::set_stencilOpStencilDepthPass}
    , stencilMask{this, &_StencilState::get_stencilMask,
                  &_StencilState::set_stencilMask}
    , stencilTest{this, &_StencilState::get_stencilTest,
                  &_StencilState::set_stencilTest}
    , _isStencilTestDirty{false}
    , _isStencilMaskDirty{false}
    , _isStencilFuncDirty{false}
    , _isStencilOpDirty{false}
{
  reset();
}

_StencilState::~_StencilState()
{
}

bool _StencilState::get_isDirty() const
{
  return _isStencilTestDirty || _isStencilMaskDirty || _isStencilFuncDirty
         || _isStencilOpDirty;
}

unsigned int _StencilState::get_stencilFunc() const
{
  return _stencilFunc;
}

void _StencilState::set_stencilFunc(unsigned int value)
{
  if (_stencilFunc == value) {
    return;
  }

  _stencilFunc        = value;
  _isStencilFuncDirty = true;
}

int _StencilState::get_stencilFuncRef() const
{
  return _stencilFuncRef;
}

void _StencilState::set_stencilFuncRef(int value)
{
  if (_stencilFuncRef == value) {
    return;
  }

  _stencilFuncRef     = value;
  _isStencilFuncDirty = true;
}

unsigned int _StencilState::get_stencilFuncMask() const
{
  return _stencilFuncMask;
}

void _StencilState::set_stencilFuncMask(unsigned int value)
{
  if (_stencilFuncMask == value) {
    return;
  }

  _stencilFuncMask    = value;
  _isStencilFuncDirty = true;
}

unsigned int _StencilState::get_stencilOpStencilFail() const
{
  return _stencilOpStencilFail;
}

void _StencilState::set_stencilOpStencilFail(unsigned int value)
{
  if (_stencilOpStencilFail == value) {
    return;
  }

  _stencilOpStencilFail = value;
  _isStencilOpDirty     = true;
}

unsigned int _StencilState::get_stencilOpDepthFail() const
{
  return _stencilOpDepthFail;
}

void _StencilState::set_stencilOpDepthFail(unsigned int value)
{
  if (_stencilOpDepthFail == value) {
    return;
  }

  _stencilOpDepthFail = value;
  _isStencilOpDirty   = true;
}

unsigned int _StencilState::get_stencilOpStencilDepthPass() const
{
  return _stencilOpStencilDepthPass;
}

void _StencilState::set_stencilOpStencilDepthPass(unsigned int value)
{
  if (_stencilOpStencilDepthPass == value) {
    return;
  }

  _stencilOpStencilDepthPass = value;
  _isStencilOpDirty          = true;
}

unsigned int _StencilState::get_stencilMask() const
{
  return _stencilMask;
}

void _StencilState::set_stencilMask(unsigned int value)
{
  if (_stencilMask == value) {
    return;
  }

  _stencilMask        = value;
  _isStencilMaskDirty = true;
}

bool _StencilState::get_stencilTest() const
{
  return _stencilTest;
}

void _StencilState::set_stencilTest(bool value)
{
  if (_stencilTest == value) {
    return;
  }

  _stencilTest        = value;
  _isStencilTestDirty = true;
}

void _StencilState::reset()
{
  _stencilTest = false;
  _stencilMask = 0xFF;

  _stencilFunc     = _StencilState::ALWAYS;
  _stencilFuncRef  = 1;
  _stencilFuncMask = 0xFF;

  _stencilOpStencilFail      = _StencilState::KEEP;
  _stencilOpDepthFail        = _StencilState::KEEP;
  _stencilOpStencilDepthPass = _StencilState::REPLACE;

  _isStencilTestDirty = true;
  _isStencilMaskDirty = true;
  _isStencilFuncDirty = true;
  _isStencilOpDirty   = true;
}

void _StencilState::apply(GL::IGLRenderingContext& gl)
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
    gl.stencilOp(stencilOpStencilFail(), stencilOpDepthFail(),
                 stencilOpStencilDepthPass());
    _isStencilOpDirty = false;
  }
}

} // end of namespace BABYLON
