#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_ANISOTROPY_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_ANISOTROPY_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(AnisotropyBlock)
FWD_CLASS_SPTR(NodeMaterialConnectionPoint)

/**
 * @brief Block used to implement the reflectivity module of the PBR material.
 */
class BABYLON_SHARED_EXPORT AnisotropyBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static AnisotropyBlockPtr New(Ts&&... args)
  {
    auto anisotropyBlock
      = std::shared_ptr<AnisotropyBlock>(new AnisotropyBlock(std::forward<Ts>(args)...));
    AnisotropyBlock::RegisterConnections(anisotropyBlock);

    return anisotropyBlock;
  }
  ~AnisotropyBlock() override;

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const AnisotropyBlockPtr& anisotropyBlock);

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
   * @brief Gets the main code of the block (fragment side)
   * @param state current state of the node material building
   * @param generateTBNSpace if true, the code needed to create the TBN coordinate space is
   * generated
   * @returns the shader code
   */
  std::string getCode(NodeMaterialBuildState& state, bool generateTBNSpace = false) const;

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
   * @brief Creates a new AnisotropyBlock.
   * @param name defines the block name
   */
  AnisotropyBlock(const std::string& name);

  /**
   * @brief Gets the intensity input component
   */
  NodeMaterialConnectionPointPtr& get_intensity();

  /**
   * @brief Gets the direction input component
   */
  NodeMaterialConnectionPointPtr& get_direction();

  /**
   * @brief Gets the uv input component
   */
  NodeMaterialConnectionPointPtr& get_uv();

  /**
   * @brief Gets the worldTangent input component
   */
  NodeMaterialConnectionPointPtr& get_worldTangent();

  /**
   * @brief Gets the anisotropy object output component
   */
  NodeMaterialConnectionPointPtr& get_anisotropy();

  /**
   * @brief Hidden
   */
  AnisotropyBlock& _buildBlock(NodeMaterialBuildState& state) override;

private:
  std::string _generateTBNSpace(NodeMaterialBuildState& state) const;

public:
  /**
   * The two properties below are set by the main PBR block prior to calling methods of this class.
   * This is to avoid having to add them as inputs here whereas they are already inputs of the main
   * block, so already known. It's less burden on the user side in the editor part.
   */

  /** @hidden */
  NodeMaterialConnectionPointPtr worldPositionConnectionPoint;
  /** @hidden */
  NodeMaterialConnectionPointPtr worldNormalConnectionPoint;

  /**
   * Gets the intensity input component
   */
  ReadOnlyProperty<AnisotropyBlock, NodeMaterialConnectionPointPtr> intensity;

  /**
   * Gets the direction input component
   */
  ReadOnlyProperty<AnisotropyBlock, NodeMaterialConnectionPointPtr> direction;

  /**
   * Gets the uv input component
   */
  ReadOnlyProperty<AnisotropyBlock, NodeMaterialConnectionPointPtr> uv;

  /**
   * Gets the worldTangent input component
   */
  ReadOnlyProperty<AnisotropyBlock, NodeMaterialConnectionPointPtr> worldTangent;

  /**
   * Gets the anisotropy object output component
   */
  ReadOnlyProperty<AnisotropyBlock, NodeMaterialConnectionPointPtr> anisotropy;

}; // end of class AnisotropyBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_ANISOTROPY_BLOCK_H
