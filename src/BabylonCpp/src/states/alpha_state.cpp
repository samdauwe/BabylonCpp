#include <babylon/states/alpha_state.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

AlphaState::AlphaState()
    : isDirty{this, &AlphaState::get_isDirty}
    , alphaBlend{this, &AlphaState::get_alphaBlend, &AlphaState::set_alphaBlend}
    , _isAlphaBlendDirty{false}
    , _isBlendFunctionParametersDirty{false}
    , _isBlendEquationParametersDirty{false}
    , _isBlendConstantsDirty{false}
    , _alphaBlend{false}
{
  reset();
}

AlphaState::~AlphaState() = default;

bool AlphaState::get_isDirty() const
{
  return _isAlphaBlendDirty || _isBlendFunctionParametersDirty;
}

bool AlphaState::get_alphaBlend() const
{
  return _alphaBlend;
}

void AlphaState::set_alphaBlend(bool value)
{
  if (_alphaBlend == value) {
    return;
  }

  _alphaBlend        = value;
  _isAlphaBlendDirty = true;
}

void AlphaState::setAlphaBlendConstants(float r, float g, float b, float a)
{
  if ((_blendConstants[0] && stl_util::almost_equal(*_blendConstants[0], r))
      && (_blendConstants[1] && stl_util::almost_equal(*_blendConstants[1], g))
      && (_blendConstants[2] && stl_util::almost_equal(*_blendConstants[2], b))
      && (_blendConstants[3] && stl_util::almost_equal(*_blendConstants[3], a))) {
    return;
  }

  _blendConstants[0] = r;
  _blendConstants[1] = g;
  _blendConstants[2] = b;
  _blendConstants[3] = a;

  _isBlendConstantsDirty = true;
}

void AlphaState::setAlphaBlendFunctionParameters(unsigned int value0, unsigned int value1,
                                                 unsigned int value2, unsigned int value3)
{
  if ((_blendFunctionParameters[0] && *_blendFunctionParameters[0] == value0)
      && (_blendFunctionParameters[1] && *_blendFunctionParameters[1] == value1)
      && (_blendFunctionParameters[2] && *_blendFunctionParameters[2] == value2)
      && (_blendFunctionParameters[3] && *_blendFunctionParameters[3] == value3)) {
    return;
  }

  _blendFunctionParameters[0] = value0;
  _blendFunctionParameters[1] = value1;
  _blendFunctionParameters[2] = value2;
  _blendFunctionParameters[3] = value3;

  _isBlendFunctionParametersDirty = true;
}

void AlphaState::setAlphaEquationParameters(unsigned int rgb, unsigned int alpha)
{
  if ((_blendEquationParameters[0] && *_blendEquationParameters[0] == rgb)
      && (_blendEquationParameters[1] && *_blendEquationParameters[1] == alpha)) {
    return;
  }

  _blendEquationParameters[0] = rgb;
  _blendEquationParameters[1] = alpha;

  _isBlendEquationParametersDirty = true;
}

void AlphaState::reset()
{
  _alphaBlend                 = false;
  _blendFunctionParameters[0] = std::nullopt;
  _blendFunctionParameters[1] = std::nullopt;
  _blendFunctionParameters[2] = std::nullopt;
  _blendFunctionParameters[3] = std::nullopt;

  _blendEquationParameters[0] = std::nullopt;
  _blendEquationParameters[1] = std::nullopt;

  _blendConstants[0] = std::nullopt;
  _blendConstants[1] = std::nullopt;
  _blendConstants[2] = std::nullopt;
  _blendConstants[3] = std::nullopt;

  _isAlphaBlendDirty              = true;
  _isBlendFunctionParametersDirty = false;
  _isBlendEquationParametersDirty = false;
  _isBlendConstantsDirty          = false;
}

void AlphaState::apply(GL::IGLRenderingContext& gl)
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
    gl.blendFuncSeparate(*_blendFunctionParameters[0], *_blendFunctionParameters[1],
                         *_blendFunctionParameters[2], *_blendFunctionParameters[3]);
    _isBlendFunctionParametersDirty = false;
  }

  // Alpha equation
  if (_isBlendEquationParametersDirty) {
    gl.blendEquationSeparate(*_blendEquationParameters[0], *_blendEquationParameters[1]);
    _isBlendEquationParametersDirty = false;
  }

  // Constants
  if (_isBlendConstantsDirty) {
    gl.blendColor(*_blendConstants[0], *_blendConstants[1], *_blendConstants[2],
                  *_blendConstants[3]);
    _isBlendConstantsDirty = false;
  }
}

} // end of namespace BABYLON
