#ifndef BABYLON_MATERIALS_MATERIAL_STENCIL_STATE__H
#define BABYLON_MATERIALS_MATERIAL_STENCIL_STATE__H

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/states/istencil_state.h>

using json = nlohmann::json;

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(MaterialStencilState)

/**
 * @brief Class that holds the different stencil states of a material.
 * Usage example: https://playground.babylonjs.com/#CW5PRI#10
 */
class BABYLON_SHARED_EXPORT MaterialStencilState : public IStencilState {

public:
  /**
   * @brief Creates a material stencil state instance.
   */
  template <typename... Ts>
  static MaterialStencilStatePtr New(Ts&&... args)
  {
    auto materialStencilState
      = std::shared_ptr<MaterialStencilState>(new MaterialStencilState(std::forward<Ts>(args)...));
    materialStencilState->reset();

    return materialStencilState;
  }
  ~MaterialStencilState(); // = default

  /**
   * @brief Resets all the stencil states to default values.
   */
  void reset() override;

  /**
   * @brief Get the current class name, useful for serialization or dynamic coding.
   * @returns "MaterialStencilState"
   */
  std::string getClassName() const;

  /**
   * @brief Makes a duplicate of the current configuration into another one.
   * @param stencilState defines stencil state where to copy the info
   */
  void copyTo(MaterialStencilState& stencilState);

  /**
   * @brief Serializes this stencil configuration.
   * @returns - An object with the serialized config.
   */
  json serialize() const;

  /**
   * @brief Parses a stencil state configuration from a serialized object.
   * @param source - Serialized object.
   * @param scene Defines the scene we are parsing for
   * @param rootUrl Defines the rootUrl to load from
   */
  MaterialStencilStatePtr parse(const json& source, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Creates a material stencil state instance.
   */
  MaterialStencilState();

  /**
   * @brief Gets the stencil function.
   */
  unsigned int get_func() const override;

  /**
   * @brief Sets the stencil function.
   */
  void set_func(unsigned int value) override;

  /**
   * @brief Gets the stencil function reference.
   */
  unsigned int get_funcRef() const override;

  /**
   * @brief Sets the stencil function reference.
   */
  void set_funcRef(unsigned int value) override;

  /**
   * @brief Gets the stencil function mask.
   */
  unsigned int get_funcMask() const override;

  /**
   * @brief Sets the stencil function mask.
   */
  void set_funcMask(unsigned int value) override;

  /**
   * @brief Gets the operation when the stencil test fails.
   */
  unsigned int get_opStencilFail() const override;

  /**
   * @brief Sets the operation when the stencil test fails.
   */
  void set_opStencilFail(unsigned int value) override;

  /**
   * @brief Gets the operation when the depth test fails.
   */
  unsigned int get_opDepthFail() const override;

  /**
   * @brief Sets the operation when the depth test fails.
   */
  void set_opDepthFail(unsigned int value) override;

  /**
   *@brief Gets the operation when the stencil+depth test succeeds.
   */
  unsigned int get_opStencilDepthPass() const override;

  /**
   * @brief Sets the operation when the stencil+depth test succeeds.
   */
  void set_opStencilDepthPass(unsigned int value) override;

  /**
   * @brief Gets the stencil mask.
   */
  unsigned int get_mask() const override;

  /**
   * @brief Sets the stencil mask.
   */
  void set_mask(unsigned int value) override;

  /**
   * @brief Enables or disables the stencil test.
   */
  bool get_enabled() const override;

  /**
   * @brief Enables or disables the stencil test.
   */
  void set_enabled(bool value) override;

private:
  unsigned int _func;
  unsigned int _funcRef;
  unsigned int _funcMask;

  unsigned int _opStencilFail;
  unsigned int _opDepthFail;
  unsigned int _opStencilDepthPass;

  unsigned int _mask;
  bool _enabled;

}; // end of class MaterialStencilState

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_STENCIL_STATE__H
