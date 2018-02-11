#ifndef BABYLON_STENCIL_STATE_H
#define BABYLON_STENCIL_STATE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT _StencilState {

public:
  /**
   * Initializes the state.
   */
  _StencilState();
  ~_StencilState();

  bool isDirty() const;
  unsigned int stencilFunc() const;
  void setStencilFunc(unsigned int value);
  int stencilFuncRef() const;
  void setStencilFuncRef(int value);
  unsigned int stencilFuncMask() const;
  void setStencilFuncMask(unsigned int value);
  unsigned int stencilOpStencilFail() const;
  void setStencilOpStencilFail(unsigned int value);
  unsigned int stencilOpDepthFail() const;
  void setStencilOpDepthFail(unsigned int value);
  unsigned int stencilOpStencilDepthPass() const;
  void setStencilOpStencilDepthPass(unsigned int value);
  unsigned int stencilMask() const;
  void setStencilMask(unsigned int value);
  bool stencilTest() const;
  void setStencilTest(bool value);
  void reset();
  void apply(GL::IGLRenderingContext& gl);

private:
  bool _isStencilTestDirty;
  bool _isStencilMaskDirty;
  bool _isStencilFuncDirty;
  bool _isStencilOpDirty;

  bool _stencilTest;

  unsigned int _stencilMask;

  unsigned int _stencilFunc;
  int _stencilFuncRef;
  unsigned int _stencilFuncMask;

  unsigned int _stencilOpStencilFail;
  unsigned int _stencilOpDepthFail;
  unsigned int _stencilOpStencilDepthPass;

}; // end of class _StencilState

} // end of namespace BABYLON

#endif // end of BABYLON_STENCIL_STATE_H
