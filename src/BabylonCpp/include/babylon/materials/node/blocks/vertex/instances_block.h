#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_INSTANCES_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_INSTANCES_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class InstancesBlock;
using InstancesBlockPtr = std::shared_ptr<InstancesBlock>;

/**
 * @brief Block used to add support for instances.
 * @see https://doc.babylonjs.com/how_to/how_to_use_instances
 */
class BABYLON_SHARED_EXPORT InstancesBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static InstancesBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<InstancesBlock>(new InstancesBlock(std::forward<Ts>(args)...));
  }
  ~InstancesBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& nodeMaterial) override;

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false) override;

protected:
  /**
   * @brief Creates a new InstancesBlock.
   * @param name defines the block name
   */
  InstancesBlock(const std::string& name);

  /**
   * @brief Gets the first world row input component.
   */
  NodeMaterialConnectionPointPtr& get_world0();

  /**
   * @brief Gets the second world row input component.
   */
  NodeMaterialConnectionPointPtr& get_world1();

  /**
   * @brief Gets the third world row input component.
   */
  NodeMaterialConnectionPointPtr& get_world2();

  /**
   * @brief Gets the forth world row input component.
   */
  NodeMaterialConnectionPointPtr& get_world3();

  /**
   * @brief Gets the world input component.
   */
  NodeMaterialConnectionPointPtr& get_world();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  InstancesBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the first world row input component
   */
  ReadOnlyProperty<InstancesBlock, NodeMaterialConnectionPointPtr> world0;

  /**
   * Gets the second world row input component
   */
  ReadOnlyProperty<InstancesBlock, NodeMaterialConnectionPointPtr> world1;

  /**
   * Gets the third world row input component
   */
  ReadOnlyProperty<InstancesBlock, NodeMaterialConnectionPointPtr> world2;

  /**
   * Gets the forth world row input component
   */
  ReadOnlyProperty<InstancesBlock, NodeMaterialConnectionPointPtr> world3;

  /**
   * Gets the world input component
   */
  ReadOnlyProperty<InstancesBlock, NodeMaterialConnectionPointPtr> world;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<InstancesBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class InstancesBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_INSTANCES_BLOCK_H
