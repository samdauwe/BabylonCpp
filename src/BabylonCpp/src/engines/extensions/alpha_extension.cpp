#include <babylon/engines/extensions/alpha_extension.h>

#include <babylon/engines/thin_engine.h>
#include <babylon/states/alpha_state.h>
#include <babylon/states/depth_culling_state.h>

namespace BABYLON {

AlphaExtension::AlphaExtension(ThinEngine* engine) : _this{engine}
{
}

AlphaExtension::~AlphaExtension() = default;

void AlphaExtension::setAlphaConstants(float r, float g, float b, float a)
{
  _this->_alphaState->setAlphaBlendConstants(r, g, b, a);
}

void AlphaExtension::setAlphaMode(unsigned int mode, bool noDepthWriteChange)
{
  if (_this->_alphaMode == mode) {
    return;
  }

  switch (mode) {
    case Constants::ALPHA_DISABLE:
      _this->_alphaState->alphaBlend = false;
      break;
    case Constants::ALPHA_PREMULTIPLIED:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE_MINUS_SRC_ALPHA, GL::ONE,
                                                          GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_PREMULTIPLIED_PORTERDUFF:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE_MINUS_SRC_ALPHA, GL::ONE,
                                                          GL::ONE_MINUS_SRC_ALPHA);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_COMBINE:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::SRC_ALPHA, GL::ONE_MINUS_SRC_ALPHA,
                                                          GL::ONE, GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_ONEONE:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE, GL::ZERO, GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_ADD:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::SRC_ALPHA, GL::ONE, GL::ZERO,
                                                          GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_SUBTRACT:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ZERO, GL::ONE_MINUS_SRC_COLOR,
                                                          GL::ONE, GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_MULTIPLY:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::DST_COLOR, GL::ZERO, GL::ONE,
                                                          GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_MAXIMIZED:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::SRC_ALPHA, GL::ONE_MINUS_SRC_COLOR,
                                                          GL::ONE, GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_INTERPOLATE:
      _this->_alphaState->setAlphaBlendFunctionParameters(
        GL::CONSTANT_COLOR, GL::ONE_MINUS_CONSTANT_COLOR, GL::CONSTANT_ALPHA,
        GL::ONE_MINUS_CONSTANT_ALPHA);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_SCREENMODE:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE_MINUS_SRC_COLOR, GL::ONE,
                                                          GL::ONE_MINUS_SRC_ALPHA);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_ONEONE_ONEONE:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE, GL::ONE, GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_ALPHATOCOLOR:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::DST_ALPHA, GL::ONE, GL::ZERO,
                                                          GL::ZERO);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_REVERSEONEMINUS:
      _this->_alphaState->setAlphaBlendFunctionParameters(
        GL::ONE_MINUS_DST_COLOR, GL::ONE_MINUS_SRC_COLOR, GL::ONE_MINUS_DST_ALPHA,
        GL::ONE_MINUS_SRC_ALPHA);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_SRC_DSTONEMINUSSRCALPHA:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE_MINUS_SRC_ALPHA, GL::ONE,
                                                          GL::ONE_MINUS_SRC_ALPHA);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_ONEONE_ONEZERO:
      _this->_alphaState->setAlphaBlendFunctionParameters(GL::ONE, GL::ONE, GL::ONE, GL::ZERO);
      _this->_alphaState->alphaBlend = true;
      break;
    case Constants::ALPHA_EXCLUSION:
      _this->_alphaState->setAlphaBlendFunctionParameters(
        GL::ONE_MINUS_DST_COLOR, GL::ONE_MINUS_SRC_COLOR, GL::ZERO, GL::ONE);
      _this->_alphaState->alphaBlend = true;
      break;
  }
  if (!noDepthWriteChange) {
    _this->depthCullingState()->depthMask = (mode == Constants::ALPHA_DISABLE);
  }
  _this->_alphaMode = mode;
}

unsigned int AlphaExtension::getAlphaMode() const
{
  return _this->_alphaMode;
}

void AlphaExtension::setAlphaEquation(unsigned int equation)
{
  if (_this->_alphaEquation == equation) {
    return;
  }

  switch (equation) {
    case Constants::ALPHA_EQUATION_ADD:
      _this->_alphaState->setAlphaEquationParameters(GL::FUNC_ADD, GL::FUNC_ADD);
      break;
    case Constants::ALPHA_EQUATION_SUBSTRACT:
      _this->_alphaState->setAlphaEquationParameters(GL::FUNC_SUBTRACT, GL::FUNC_SUBTRACT);
      break;
    case Constants::ALPHA_EQUATION_REVERSE_SUBTRACT:
      _this->_alphaState->setAlphaEquationParameters(GL::FUNC_REVERSE_SUBTRACT,
                                                     GL::FUNC_REVERSE_SUBTRACT);
      break;
    case Constants::ALPHA_EQUATION_MAX:
      _this->_alphaState->setAlphaEquationParameters(GL::MAX, GL::MAX);
      break;
    case Constants::ALPHA_EQUATION_MIN:
      _this->_alphaState->setAlphaEquationParameters(GL::MIN, GL::MIN);
      break;
    case Constants::ALPHA_EQUATION_DARKEN:
      _this->_alphaState->setAlphaEquationParameters(GL::MIN, GL::FUNC_ADD);
      break;
  }
  _this->_alphaEquation = equation;
}

unsigned int AlphaExtension::getAlphaEquation() const
{
  return _this->_alphaEquation;
}

} // end of namespace BABYLON
