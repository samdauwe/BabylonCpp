#ifndef BABYLON_ALPHA_STATE_H
#define BABYLON_ALPHA_STATE_H

#include <array>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

namespace GL {
class IGLRenderingContext;
} // end of namespace GL

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
  std::array<std::optional<unsigned int>, 4> _blendFunctionParameters;
  std::array<std::optional<unsigned int>, 2> _blendEquationParameters;
  std::array<std::optional<float>, 4> _blendConstants;

}; // end of class _AlphaState

} // end of namespace BABYLON

#endif // end of BABYLON_ALPHA_STATE_H
