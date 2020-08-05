#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_BONES_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_BONES_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class BonesBlock;
using BonesBlockPtr = std::shared_ptr<BonesBlock>;

/**
 * @brief Block used to add support for vertex skinning (bones).
 */
class BABYLON_SHARED_EXPORT BonesBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static BonesBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<BonesBlock>(new BonesBlock(std::forward<Ts>(args)...));
  }
  ~BonesBlock() override;

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  void initialize(NodeMaterialBuildState& state) override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& material) override;

  /**
   * @brief Add potential fallbacks if shader compilation fails.
   * @param mesh defines the mesh to be rendered
   * @param fallbacks defines the current prioritized list of fallbacks
   */
  void provideFallbacks(AbstractMesh* mesh, EffectFallbacks* fallbacks) override;

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable
   * === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false,
                      SubMesh* subMesh = nullptr) override;

protected:
  /**
   * @brief Creates a new BonesBlock.
   * @param name defines the block name
   */
  BonesBlock(const std::string& name);

  /**
   * @brief Gets the matrix indices input component.
   */
  NodeMaterialConnectionPointPtr& get_matricesIndices();

  /**
   * @brief Gets the matrix weights input component.
   */
  NodeMaterialConnectionPointPtr& get_matricesWeights();

  /**
   * @brief Gets the matrix weights input component.
   */
  NodeMaterialConnectionPointPtr& get_matricesIndicesExtra();

  /**
   * @brief Gets the extra matrix weights input component.
   */
  NodeMaterialConnectionPointPtr& get_matricesWeightsExtra();

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
  BonesBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the matrix indices input component
   */
  ReadOnlyProperty<BonesBlock, NodeMaterialConnectionPointPtr> matricesIndices;

  /**
   * Gets the matrix weights input component
   */
  ReadOnlyProperty<BonesBlock, NodeMaterialConnectionPointPtr> matricesWeights;

  /**
   * Gets the matrix weights input component
   */
  ReadOnlyProperty<BonesBlock, NodeMaterialConnectionPointPtr> matricesIndicesExtra;

  /**
   * Gets the extra matrix weights input component
   */
  ReadOnlyProperty<BonesBlock, NodeMaterialConnectionPointPtr> matricesWeightsExtra;

  /**
   * Gets the world input component
   */
  ReadOnlyProperty<BonesBlock, NodeMaterialConnectionPointPtr> world;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<BonesBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class BonesBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_BONES_BLOCK_H
