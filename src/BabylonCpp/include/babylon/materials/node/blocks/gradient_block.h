#ifndef BABYLON_MATERIALS_NODE_BLOCKS_GRADIENT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_GRADIENT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>
#include <babylon/maths/color3.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

FWD_CLASS_SPTR(GradientBlock)

/**
 * @brief Class used to store a color step for the GradientBlock.
 */
class BABYLON_SHARED_EXPORT GradientBlockColorStep {

public:
  /**
   * @brief Creates a new GradientBlockColorStep.
   * @param step defines a value indicating which step this color is associated with (between 0 and
   * 1)
   * @param color defines the color associated with this step
   */
  GradientBlockColorStep(float iStep, const Color3& iColor);
  GradientBlockColorStep(const GradientBlockColorStep& other);
  ~GradientBlockColorStep() = default;

protected:
  /**
   * @brief Gets value indicating which step this color is associated with (between 0 and 1).
   */
  float get_step() const;

  /**
   * @brief Sets a value indicating which step this color is associated with (between 0 and 1).
   */
  void set_step(float val);

  /**
   * @brief Gets the color associated with this step.
   */
  Color3& get_color();

  /**
   * @brief Sets the color associated with this step.
   */
  void set_color(const Color3& val);

public:
  /**
   * Gets or sets the value indicating which step this color is associated with (between 0 and 1)
   */
  Property<GradientBlockColorStep, float> step;

  /**
   * Gets or sets the color associated with this step
   */
  Property<GradientBlockColorStep, Color3> color;

private:
  /**
   * Gets or sets a value indicating which step this color is associated with (between 0 and 1)
   */
  float _step;
  /**
   * Gets or sets the color associated with this step
   */
  Color3 _color;
}; // end of structGradientBlockColorStep

/**
 * @brief Block used to return a color from a gradient based on an input value between 0 and 1.
 */
class BABYLON_SHARED_EXPORT GradientBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static GradientBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<GradientBlock>(new GradientBlock(std::forward<Ts>(args)...));
  }
  ~GradientBlock() override;

  /**
   * @brief Calls observable when the value is changed
   */
  void colorStepsUpdated();

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Serializes this block in a JSON representation.
   * @returns the serialized block object
   */
  json serialize() const override;

  /**
   * @brief Hidden
   */
  void _deserialize(const json& serializationObject, Scene* scene,
                    const std::string& rootUrl) override;

protected:
  /**
   * @brief Creates a new GradientBlock.
   * @param name defines the block name
   */
  GradientBlock(const std::string& name);

  /**
   * @brief Gets the gradient input component.
   */
  NodeMaterialConnectionPointPtr& get_gradient();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  GradientBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

private:
  std::string _writeColorConstant(unsigned int);

public:
  /**
   * Gets or sets the list of color steps
   */
  std::vector<GradientBlockColorStep> colorSteps;

  /**
   * Gets an observable raised when the value is changed
   */
  Observable<GradientBlock> onValueChangedObservable;

  /**
   * Gets the gradient input component
   */
  ReadOnlyProperty<GradientBlock, NodeMaterialConnectionPointPtr> gradient;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<GradientBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class GradientBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_GRADIENT_BLOCK_H
