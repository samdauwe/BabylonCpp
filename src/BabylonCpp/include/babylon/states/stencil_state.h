#ifndef BABYLON_STENCIL_STATE_H
#define BABYLON_STENCIL_STATE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/engines/constants.h>
#include <babylon/states/istencil_state.h>

namespace BABYLON {

FWD_CLASS_SPTR(StencilState)

class BABYLON_SHARED_EXPORT StencilState : public IStencilState {

public:
  /**
   * Passed to depthFunction or stencilFunction to specify depth or stencil tests will always pass.
   * i.e. Pixels will be drawn in the order they are drawn
   */
  static constexpr unsigned int ALWAYS = Constants::ALWAYS;
  /**
   * Passed to stencilOperation to specify that stencil value must be kept
   */
  static constexpr unsigned int KEEP = Constants::KEEP;
  /**
   * Passed to stencilOperation to specify that stencil value must be replaced
   */
  static constexpr unsigned int REPLACE = Constants::REPLACE;

public:
  /**
   * @brief Creates a material stencil state instance.
   */
  template <typename... Ts>
  static StencilStatePtr New(Ts&&... args)
  {
    auto stencilState = std::shared_ptr<StencilState>(new StencilState(std::forward<Ts>(args)...));
    stencilState->reset();

    return stencilState;
  }
  ~StencilState(); // = default

public:
  /**
   * Initializes the state.
   */
  StencilState();

  void reset() override;

protected:
  unsigned int get_func() const override;
  void set_func(unsigned int value) override;
  unsigned int get_funcRef() const override;
  void set_funcRef(unsigned int value) override;
  unsigned int get_funcMask() const override;
  void set_funcMask(unsigned int value) override;
  unsigned int get_opStencilFail() const override;
  void set_opStencilFail(unsigned int value) override;
  unsigned int get_opDepthFail() const override;
  void set_opDepthFail(unsigned int value) override;
  unsigned int get_opStencilDepthPass() const override;
  void set_opStencilDepthPass(unsigned int value) override;
  unsigned int get_mask() const override;
  void set_mask(unsigned int value) override;
  bool get_enabled() const override;
  void set_enabled(bool value) override;

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
  Property<StencilState, unsigned int> stencilFunc;
  Property<StencilState, int> stencilFuncRef;
  Property<StencilState, unsigned int> stencilFuncMask;
  Property<StencilState, unsigned int> stencilOpStencilFail;
  Property<StencilState, unsigned int> stencilOpDepthFail;
  Property<StencilState, unsigned int> stencilOpStencilDepthPass;
  Property<StencilState, unsigned int> stencilMask;
  Property<StencilState, bool> stencilTest;

private:
  unsigned int _func;
  unsigned int _funcRef;
  unsigned int _funcMask;
  unsigned int _opStencilFail;
  unsigned int _opDepthFail;
  unsigned int _opStencilDepthPass;
  unsigned int _mask;
  bool _enabled;

}; // end of class StencilState

} // end of namespace BABYLON

#endif // end of BABYLON_STENCIL_STATE_H
