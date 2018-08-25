#ifndef BABYLON_ALPHA_STATE_H
#define BABYLON_ALPHA_STATE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT _AlphaState {

public:
  /**
   * @brief Initializes the state.
   */
  _AlphaState();
  ~_AlphaState();

  void setAlphaBlendConstants(float r, float g, float b, float a);
  void setAlphaBlendFunctionParameters(unsigned int value0, unsigned int value1,
                                       unsigned int value2,
                                       unsigned int value3);
  void setAlphaEquationParameters(unsigned int rgb, unsigned int alpha);
  void reset();
  void apply(GL::IGLRenderingContext& gl);

protected:
  bool get_isDirty() const;
  bool get_alphaBlend() const;
  void set_alphaBlend(bool value);

public:
  ReadOnlyProperty<_AlphaState, bool> isDirty;
  Property<_AlphaState, bool> alphaBlend;

private:
  bool _isAlphaBlendDirty;
  bool _isBlendFunctionParametersDirty;
  bool _isBlendEquationParametersDirty;
  bool _isBlendConstantsDirty;
  bool _alphaBlend;
  array_t<nullable_t<unsigned int>, 4> _blendFunctionParameters;
  array_t<nullable_t<unsigned int>, 2> _blendEquationParameters;
  array_t<nullable_t<float>, 4> _blendConstants;

}; // end of class _AlphaState

} // end of namespace BABYLON

#endif // end of BABYLON_ALPHA_STATE_H
