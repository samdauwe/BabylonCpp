#ifndef BABYLON_ISTENCIL_STATE_H
#define BABYLON_ISTENCIL_STATE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT IStencilState {

public:
  IStencilState();
  virtual ~IStencilState(); // = default

public:
  /**
   * Enables or disables the stencil test
   */
  Property<IStencilState, bool> enabled;

  /**
   * Gets or sets the stencil mask
   */
  Property<IStencilState, unsigned int> mask;

  /**
   * Gets or sets the stencil function
   */
  Property<IStencilState, unsigned int> func;
  /**
   * Gets or sets the stencil function reference
   */
  Property<IStencilState, unsigned int> funcRef;
  /**
   * Gets or sets the stencil function mask
   */
  Property<IStencilState, unsigned int> funcMask;

  /**
   * Gets or sets the operation when the stencil+depth test succeeds
   */
  Property<IStencilState, unsigned int> opStencilDepthPass;
  /**
   * Gets or sets the operation when the stencil test fails
   */
  Property<IStencilState, unsigned int> opStencilFail;
  /**
   * Gets or sets the operation when the depth test fails
   */
  Property<IStencilState, unsigned int> opDepthFail;

  virtual void reset() = 0;

protected:
  /**
   * @brief Enables or disables the stencil test.
   */
  virtual bool get_enabled() const = 0;

  /**
   * @brief Enables or disables the stencil test.
   */
  virtual void set_enabled(bool value) = 0;

  /**
   * @brief Gets the stencil mask.
   */
  virtual unsigned int get_mask() const = 0;

  /**
   * @brief Sets the stencil mask.
   */
  virtual void set_mask(unsigned int value) = 0;

  /**
   * @brief Gets the stencil function.
   */
  virtual unsigned int get_func() const = 0;

  /**
   * @brief Sets the stencil function.
   */
  virtual void set_func(unsigned int value) = 0;

  /**
   * @brief Gets the stencil function reference.
   */
  virtual unsigned int get_funcRef() const = 0;

  /**
   * @brief Sets the stencil function reference.
   */
  virtual void set_funcRef(unsigned int value) = 0;

  /**
   * @brief Gets the stencil function mask.
   */
  virtual unsigned int get_funcMask() const = 0;

  /**
   * @brief Sets the stencil function mask.
   */
  virtual void set_funcMask(unsigned int value) = 0;

  /**
   *@brief Gets the operation when the stencil+depth test succeeds.
   */
  virtual unsigned int get_opStencilDepthPass() const = 0;

  /**
   * @brief Sets the operation when the stencil+depth test succeeds.
   */
  virtual void set_opStencilDepthPass(unsigned int value) = 0;

  /**
   * @brief Gets the operation when the stencil test fails.
   */
  virtual unsigned int get_opStencilFail() const = 0;

  /**
   * @brief Sets the operation when the stencil test fails.
   */
  virtual void set_opStencilFail(unsigned int value) = 0;

  /**
   * @brief Gets the operation when the depth test fails.
   */
  virtual unsigned int get_opDepthFail() const = 0;

  /**
   * @brief Sets the operation when the depth test fails.
   */
  virtual void set_opDepthFail(unsigned int value) = 0;

}; // end of class IStencilState

} // end of namespace BABYLON

#endif // end of BABYLON_ISTENCIL_STATE_H
