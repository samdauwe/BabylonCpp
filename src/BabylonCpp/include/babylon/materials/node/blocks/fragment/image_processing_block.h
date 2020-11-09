#ifndef BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_IMAGE_PROCESSING_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_IMAGE_PROCESSING_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class ImageProcessingBlock;
using ImageProcessingBlockPtr = std::shared_ptr<ImageProcessingBlock>;

/**
 * @brief Block used to add image processing support to fragment shader.
 */
class BABYLON_SHARED_EXPORT ImageProcessingBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ImageProcessingBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ImageProcessingBlock>(
      new ImageProcessingBlock(std::forward<Ts>(args)...));
  }
  ~ImageProcessingBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Initialize the block and prepare the context for build.
   * @param state defines the state that will be used for the build
   */
  void initialize(NodeMaterialBuildState& state) override;

  /**
   * @brief Checks if the block is ready.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   * @returns true if the block is ready
   */
  bool isReady(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
               const NodeMaterialDefines& defines, bool useInstances = false) override;

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

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

protected:
  /**
   * @brief Creates a new ImageProcessingBlock.
   * @param name defines the block name
   */
  ImageProcessingBlock(const std::string& name);

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the output component.
   */
  NodeMaterialConnectionPointPtr& get_output();

  /**
   * @brief Hidden
   */
  ImageProcessingBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the color input component
   */
  ReadOnlyProperty<ImageProcessingBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the output component
   */
  ReadOnlyProperty<ImageProcessingBlock, NodeMaterialConnectionPointPtr> output;

}; // end of class ImageProcessingBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_FRAGMENT_IMAGE_PROCESSING_BLOCK_H
