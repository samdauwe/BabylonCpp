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

  void reset();
  void apply(GL::IGLRenderingContext& gl);

protected:
  bool get_isDirty() const;
  unsigned int get_stencilFunc() const;
  void set_stencilFunc(unsigned int value);
  int get_stencilFuncRef() const;
  void set_stencilFuncRef(int value);
  unsigned int get_stencilFuncMask() const;
  void set_stencilFuncMask(unsigned int value);
  unsigned int get_stencilOpStencilFail() const;
  void set_stencilOpStencilFail(unsigned int value);
  unsigned int get_stencilOpDepthFail() const;
  void set_stencilOpDepthFail(unsigned int value);
  unsigned int get_stencilOpStencilDepthPass() const;
  void set_stencilOpStencilDepthPass(unsigned int value);
  unsigned int get_stencilMask() const;
  void set_stencilMask(unsigned int value);
  bool get_stencilTest() const;
  void set_stencilTest(bool value);

public:
  ReadOnlyProperty<_StencilState, bool> isDirty;
  Property<_StencilState, unsigned int> stencilFunc;
  Property<_StencilState, int> stencilFuncRef;
  Property<_StencilState, unsigned int> stencilFuncMask;
  Property<_StencilState, unsigned int> stencilOpStencilFail;
  Property<_StencilState, unsigned int> stencilOpDepthFail;
  Property<_StencilState, unsigned int> stencilOpStencilDepthPass;
  Property<_StencilState, unsigned int> stencilMask;
  Property<_StencilState, bool> stencilTest;

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
