#ifndef BABYLON_ALPHA_STATE_H
#define BABYLON_ALPHA_STATE_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT _AlphaState {

public:
  /**
   * @brief Initializes the state.
   */
  _AlphaState();
  ~_AlphaState();

  bool isDirty() const;
  bool alphaBlend() const;
  void setAlphaBlend(bool value);
  void setAlphaBlendConstants(float r, float g, float b, float a);
  void setAlphaBlendFunctionParameters(unsigned int value0, unsigned int value1,
                                       unsigned int value2,
                                       unsigned int value3);
  void setAlphaEquationParameters(unsigned int rgb, unsigned int alpha);
  void reset();
  void apply(GL::IGLRenderingContext& gl);

private:
  bool _isAlphaBlendDirty;
  bool _isBlendFunctionParametersDirty;
  bool _isBlendEquationParametersDirty;
  bool _isBlendConstantsDirty;
  bool _alphaBlend;
  array_t<Nullable<unsigned int>, 4> _blendFunctionParameters;
  array_t<Nullable<unsigned int>, 2> _blendEquationParameters;
  array_t<Nullable<float>, 4> _blendConstants;

}; // end of class _AlphaState

} // end of namespace BABYLON

#endif // end of BABYLON_ALPHA_STATE_H
