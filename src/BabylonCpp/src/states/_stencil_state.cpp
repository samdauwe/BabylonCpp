#include <babylon/states/_stencil_state.h>

#include <babylon/engine/engine.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {
namespace Internals {

_StencilState::_StencilState()
    : _isStencilTestDirty{false}
    , _isStencilMaskDirty{false}
    , _isStencilFuncDirty{false}
    , _isStencilOpDirty{false}
{
  reset();
}

_StencilState::~_StencilState()
{
}

bool _StencilState::isDirty() const
{
  return _isStencilTestDirty || _isStencilMaskDirty || _isStencilFuncDirty
         || _isStencilOpDirty;
}

unsigned int _StencilState::stencilFunc() const
{
  return _stencilFunc;
}

void _StencilState::setStencilFunc(unsigned int value)
{
  if (_stencilFunc == value) {
    return;
  }

  _stencilFunc        = value;
  _isStencilFuncDirty = true;
}

int _StencilState::stencilFuncRef() const
{
  return _stencilFuncRef;
}

void _StencilState::setStencilFuncRef(int value)
{
  if (_stencilFuncRef == value) {
    return;
  }

  _stencilFuncRef     = value;
  _isStencilFuncDirty = true;
}

unsigned int _StencilState::stencilFuncMask() const
{
  return _stencilFuncMask;
}

void _StencilState::setStencilFuncMask(unsigned int value)
{
  if (_stencilFuncMask == value) {
    return;
  }

  _stencilFuncMask    = value;
  _isStencilFuncDirty = true;
}

unsigned int _StencilState::stencilOpStencilFail() const
{
  return _stencilOpStencilFail;
}

void _StencilState::setStencilOpStencilFail(unsigned int value)
{
  if (_stencilOpStencilFail == value) {
    return;
  }

  _stencilOpStencilFail = value;
  _isStencilOpDirty     = true;
}

unsigned int _StencilState::stencilOpDepthFail() const
{
  return _stencilOpDepthFail;
}

void _StencilState::setStencilOpDepthFail(unsigned int value)
{
  if (_stencilOpDepthFail == value) {
    return;
  }

  _stencilOpDepthFail = value;
  _isStencilOpDirty   = true;
}

unsigned int _StencilState::stencilOpStencilDepthPass() const
{
  return _stencilOpStencilDepthPass;
}

void _StencilState::setStencilOpStencilDepthPass(unsigned int value)
{
  if (_stencilOpStencilDepthPass == value) {
    return;
  }

  _stencilOpStencilDepthPass = value;
  _isStencilOpDirty          = true;
}

unsigned int _StencilState::stencilMask() const
{
  return _stencilMask;
}

void _StencilState::setStencilMask(unsigned int value)
{
  if (_stencilMask == value) {
    return;
  }

  _stencilMask        = value;
  _isStencilMaskDirty = true;
}

bool _StencilState::stencilTest() const
{
  return _stencilTest;
}

void _StencilState::setStencilTest(bool value)
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

  _stencilFunc     = Engine::ALWAYS;
  _stencilFuncRef  = 1;
  _stencilFuncMask = 0xFF;

  _stencilOpStencilFail      = Engine::KEEP;
  _stencilOpDepthFail        = Engine::KEEP;
  _stencilOpStencilDepthPass = Engine::REPLACE;

  _isStencilTestDirty = true;
  _isStencilMaskDirty = true;
  _isStencilFuncDirty = true;
  _isStencilOpDirty   = true;
}

void _StencilState::apply(GL::IGLRenderingContext* gl)
{
  if (!isDirty()) {
    return;
  }

  // Stencil test
  if (_isStencilTestDirty) {
    if (stencilTest()) {
      gl->enable(GL::STENCIL_TEST);
    }
    else {
      gl->disable(GL::STENCIL_TEST);
    }
    _isStencilTestDirty = false;
  }

  // Stencil mask
  if (_isStencilMaskDirty) {
    gl->stencilMask(stencilMask());
    _isStencilMaskDirty = false;
  }

  // Stencil func
  if (_isStencilFuncDirty) {
    gl->stencilFunc(stencilFunc(), stencilFuncRef(), stencilFuncMask());
    _isStencilFuncDirty = false;
  }

  // Stencil op
  if (_isStencilOpDirty) {
    gl->stencilOp(stencilOpStencilFail(), stencilOpDepthFail(),
                  stencilOpStencilDepthPass());
    _isStencilOpDirty = false;
  }
}

} // end of namespace Internals
} // end of namespace BABYLON
