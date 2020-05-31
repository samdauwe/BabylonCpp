#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_AMBIENT_OCCLUSION_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_AMBIENT_OCCLUSION_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class AmbientOcclusionBlock;
using AmbientOcclusionBlockPtr = std::shared_ptr<AmbientOcclusionBlock>;

/**
 * @brief Block used to implement the ambient occlusion module of the PBR material.
 */
class BABYLON_SHARED_EXPORT AmbientOcclusionBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static AmbientOcclusionBlockPtr New(Ts&&... args)
  {
    auto ambientOcclusionBlock = std::shared_ptr<AmbientOcclusionBlock>(
      new AmbientOcclusionBlock(std::forward<Ts>(args)...));
    AmbientOcclusionBlock::RegisterConnections(ambientOcclusionBlock);

    return ambientOcclusionBlock;
  }
  ~AmbientOcclusionBlock() override;

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const AmbientOcclusionBlockPtr& ambientOcclusionBlock);

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
   * @param block instance of an AmbientOcclusionBlock or null if the code must be generated without
   * an active ambient occlusion module
   * @returns the shader code
   */
  static std::string GetCode(const AmbientOcclusionBlockPtr& block);

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false) override;

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
   * @brief Create a new AmbientOcclusionBlock.
   * @param name defines the block name
   */
  AmbientOcclusionBlock(const std::string& name);

  /**
   * @brief Gets the texture input component.
   */
  NodeMaterialConnectionPointPtr& get_texture();

  /**
   * @brief Gets the texture intensity component.
   */
  NodeMaterialConnectionPointPtr& get_intensity();

  /**
   * @brief Gets the direct light intensity input component.
   */
  NodeMaterialConnectionPointPtr& get_directLightIntensity();

  /**
   * @brief Gets the ambient occlusion object output component.
   */
  NodeMaterialConnectionPointPtr& get_ambientOcclusion();

  /**
   * @brief Hidden
   */
  AmbientOcclusionBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /**
   * Gets the texture input component
   */
  ReadOnlyProperty<AmbientOcclusionBlock, NodeMaterialConnectionPointPtr> texture;

  /**
   * Gets the texture intensity component
   */
  ReadOnlyProperty<AmbientOcclusionBlock, NodeMaterialConnectionPointPtr> intensity;

  /**
   * Gets the direct light intensity input component
   */
  ReadOnlyProperty<AmbientOcclusionBlock, NodeMaterialConnectionPointPtr> directLightIntensity;

  /**
   * Gets the ambient occlusion object output component
   */
  ReadOnlyProperty<AmbientOcclusionBlock, NodeMaterialConnectionPointPtr> ambientOcclusion;

  /**
   * Specifies if the ambient texture contains the ambient occlusion information in its red channel
   * only.
   */
  bool useAmbientInGrayScale;

}; // end of class AmbientOcclusionBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_AMBIENT_OCCLUSION_BLOCK_H
