#ifndef BABYLON_ALPHA_STATE_H
#define BABYLON_ALPHA_STATE_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Internals {

class BABYLON_SHARED_EXPORT _AlphaState {

public:
  /**
   * Initializes the state.
   */
  _AlphaState();
  ~_AlphaState();

  bool isDirty() const;
  bool alphaBlend() const;
  void setAlphaBlend(bool value);
  void setAlphaBlendFunctionParameters(int value0, int value1, int value2,
                                       int value3);
  void reset();
  void apply(GL::IGLRenderingContext& gl);

private:
  bool _isAlphaBlendDirty;
  bool _isBlendFunctionParametersDirty;
  bool _alphaBlend;
  std::array<int, 4> _blendFunctionParameters;

}; // end of class _AlphaState

} // end of namespace Internals
} // end of namespace BABYLON

#endif // end of BABYLON_ALPHA_STATE_H
