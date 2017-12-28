#include <babylon/states/_alpha_state.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {
namespace Internals {

_AlphaState::_AlphaState()
    : _isAlphaBlendDirty{false}
    , _isBlendFunctionParametersDirty{false}
    , _isBlendEquationParametersDirty{false}
    , _isBlendConstantsDirty{false}
    , _alphaBlend{false}
{
  reset();
}

_AlphaState::~_AlphaState()
{
}

bool _AlphaState::isDirty() const
{
  return _isAlphaBlendDirty || _isBlendFunctionParametersDirty;
}

bool _AlphaState::alphaBlend() const
{
  return _alphaBlend;
}

void _AlphaState::setAlphaBlend(bool value)
{
  if (_alphaBlend == value) {
    return;
  }

  _alphaBlend        = value;
  _isAlphaBlendDirty = true;
}

void _AlphaState::setAlphaBlendConstants(float r, float g, float b, float a)
{
  if (stl_util::almost_equal(*_blendConstants[0], r)
      && stl_util::almost_equal(*_blendConstants[1], g)
      && stl_util::almost_equal(*_blendConstants[2], b)
      && stl_util::almost_equal(*_blendConstants[3], a)) {
    return;
  }

  _blendConstants[0] = r;
  _blendConstants[1] = g;
  _blendConstants[2] = b;
  _blendConstants[3] = a;

  _isBlendConstantsDirty = true;
}

void _AlphaState::setAlphaBlendFunctionParameters(unsigned int value0,
                                                  unsigned int value1,
                                                  unsigned int value2,
                                                  unsigned int value3)
{
  if (_blendFunctionParameters[0] == value0
      && _blendFunctionParameters[1] == value1
      && _blendFunctionParameters[2] == value2
      && _blendFunctionParameters[3] == value3) {
    return;
  }

  _blendFunctionParameters[0] = value0;
  _blendFunctionParameters[1] = value1;
  _blendFunctionParameters[2] = value2;
  _blendFunctionParameters[3] = value3;

  _isBlendFunctionParametersDirty = true;
}

void _AlphaState::setAlphaEquationParameters(unsigned int rgb,
                                             unsigned int alpha)
{
  if (_blendEquationParameters[0] == rgb
      && _blendEquationParameters[1] == alpha) {
    return;
  }

  _blendEquationParameters[0] = rgb;
  _blendEquationParameters[1] = alpha;

  _isBlendEquationParametersDirty = true;
}

void _AlphaState::reset()
{
  _alphaBlend                 = false;
  _blendFunctionParameters[0] = nullptr;
  _blendFunctionParameters[1] = nullptr;
  _blendFunctionParameters[2] = nullptr;
  _blendFunctionParameters[3] = nullptr;

  _blendEquationParameters[0] = nullptr;
  _blendEquationParameters[1] = nullptr;

  _blendConstants[0] = nullptr;
  _blendConstants[1] = nullptr;
  _blendConstants[2] = nullptr;
  _blendConstants[3] = nullptr;

  _isAlphaBlendDirty              = true;
  _isBlendFunctionParametersDirty = false;
  _isBlendEquationParametersDirty = false;
  _isBlendConstantsDirty          = false;
}

void _AlphaState::apply(GL::IGLRenderingContext& gl)
{
  if (!isDirty()) {
    return;
  }

  // Alpha blend
  if (_isAlphaBlendDirty) {
    if (_alphaBlend) {
      gl.enable(GL::BLEND);
    }
    else {
      gl.disable(GL::BLEND);
    }

    _isAlphaBlendDirty = false;
  }

  // Alpha function
  if (_isBlendFunctionParametersDirty) {
    gl.blendFuncSeparate(
      *_blendFunctionParameters[0], *_blendFunctionParameters[1],
      *_blendFunctionParameters[2], *_blendFunctionParameters[3]);
    _isBlendFunctionParametersDirty = false;
  }

  // Alpha equation
  if (_isBlendEquationParametersDirty) {
    gl.blendEquationSeparate(*_blendEquationParameters[0],
                             *_blendEquationParameters[1]);
    _isBlendEquationParametersDirty = false;
  }

  // Constants
  if (_isBlendConstantsDirty) {
    gl.blendColor(*_blendConstants[0], *_blendConstants[1], *_blendConstants[2],
                  *_blendConstants[3]);
    _isBlendConstantsDirty = false;
  }
}

} // end of namespace Internals
} // end of namespace BABYLON
