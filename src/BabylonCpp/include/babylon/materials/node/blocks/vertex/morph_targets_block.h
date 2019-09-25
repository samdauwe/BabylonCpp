#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_MORPH_TARGETS_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_MORPH_TARGETS_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class MorphTargetsBlock;
using MorphTargetsBlockPtr = std::shared_ptr<MorphTargetsBlock>;

/**
 * @brief Block used to add morph targets support to vertex shader.
 */
class BABYLON_SHARED_EXPORT MorphTargetsBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static MorphTargetsBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<MorphTargetsBlock>(
      new MorphTargetsBlock(std::forward<Ts>(args)...));
  }
  ~MorphTargetsBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  const std::string getClassName() const override;

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  void initialize(NodeMaterialBuildState& state) override;

  /**
   * @brief Lets the block try to connect some inputs automatically.
   * @param material defines the hosting NodeMaterial
   */
  void autoConfigure(const NodeMaterialPtr& material) override;

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines,
                      bool useInstances = false) override;
  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable
   * === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial,
            Mesh* mesh = nullptr) override;

  /**
   * @brief Function called when a block is declared as repeatable content
   * generator.
   * @param vertexShaderState defines the current compilation state for the
   * vertex shader
   * @param fragmentShaderState defines the current compilation state for the
   * fragment shader
   * @param mesh defines the mesh to be rendered
   * @param defines defines the material defines to update
   */
  void
  replaceRepeatableContent(NodeMaterialBuildState& vertexShaderState,
                           const NodeMaterialBuildState& fragmentShaderState,
                           AbstractMesh* mesh,
                           NodeMaterialDefines& defines) override;

protected:
  /**
   * @brief Creates a new MorphTargetsBlock.
   * @param name defines the block name
   */
  MorphTargetsBlock(const std::string& name);

  /**
   * @brief Gets the position input component.
   */
  NodeMaterialConnectionPointPtr& get_position();

  /**
   * @brief Gets the normal input component.
   */
  NodeMaterialConnectionPointPtr& get_normal();

  /**
   * @brief Gets the tangent input component.
   */
  NodeMaterialConnectionPointPtr& get_tangent();

  /**
   * @brief Gets the uv input component
   */
  NodeMaterialConnectionPointPtr& get_uv();

  /**
   * @brief Gets the position output component.
   */
  NodeMaterialConnectionPointPtr& get_positionOutput();

  /**
   * @brief Gets the normal output component.
   */
  NodeMaterialConnectionPointPtr& get_normalOutput();

  /**
   * @brief Gets the tangent output component.
   */
  NodeMaterialConnectionPointPtr& get_tangentOutput();

  /**
   * @brief Gets the uv output component.
   */
  NodeMaterialConnectionPointPtr& get_uvOutput();

  /**
   * @brief Hidden
   */
  MorphTargetsBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the position input component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr> position;

  /**
   * Gets the normal input component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr> normal;

  /**
   * Gets the tangent input component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr> tangent;

  /**
   * Gets the uv input component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr> uv;

  /**
   * Gets the position output component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr>
    positionOutput;

  /**
   * Gets the normal output component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr>
    normalOutput;

  /**
   * Gets the tangent output component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr>
    tangentOutput;

  /**
   * Gets the uv output component
   */
  ReadOnlyProperty<MorphTargetsBlock, NodeMaterialConnectionPointPtr> uvOutput;

private:
  std::string _repeatableContentAnchor;
  size_t _repeatebleContentGenerated;

}; // end of class MorphTargetsBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_MORPH_TARGETS_BLOCK_H
