#include <babylon/states/_alpha_state.h>

#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {
namespace Internals {

_AlphaState::_AlphaState()
    : _isAlphaBlendDirty{false}
    , _isBlendFunctionParametersDirty{false}
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

void _AlphaState::setAlphaBlendFunctionParameters(int value0, int value1,
                                                  int value2, int value3)
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
void _AlphaState::reset()
{
  _alphaBlend                 = false;
  _blendFunctionParameters[0] = -1;
  _blendFunctionParameters[1] = -1;
  _blendFunctionParameters[2] = -1;
  _blendFunctionParameters[3] = -1;

  _isAlphaBlendDirty              = true;
  _isBlendFunctionParametersDirty = false;
}

void _AlphaState::apply(GL::IGLRenderingContext* gl)
{
  if (!isDirty()) {
    return;
  }

  // Alpha blend
  if (_isAlphaBlendDirty) {
    if (_alphaBlend) {
      gl->enable(GL::BLEND);
    }
    else {
      gl->disable(GL::BLEND);
    }

    _isAlphaBlendDirty = false;
  }

  // Alpha function
  if (_isBlendFunctionParametersDirty) {
    gl->blendFuncSeparate(
      static_cast<unsigned int>(_blendFunctionParameters[0]),
      static_cast<unsigned int>(_blendFunctionParameters[1]),
      static_cast<unsigned int>(_blendFunctionParameters[2]),
      static_cast<unsigned int>(_blendFunctionParameters[3]));
    _isBlendFunctionParametersDirty = false;
  }
}

} // end of namespace Internals
} // end of namespace BABYLON
