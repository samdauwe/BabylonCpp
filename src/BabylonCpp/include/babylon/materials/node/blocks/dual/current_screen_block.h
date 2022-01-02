#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DUAL_CURRENT_SHEEN_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DUAL_CURRENT_SHEEN_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class BaseTexture;
class CurrentScreenBlock;
using BaseTexturePtr        = std::shared_ptr<BaseTexture>;
using CurrentScreenBlockPtr = std::shared_ptr<CurrentScreenBlock>;

/**
 * @brief Base block used as input for post process.
 */
class BABYLON_SHARED_EXPORT CurrentScreenBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static CurrentScreenBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<CurrentScreenBlock>(new CurrentScreenBlock(std::forward<Ts>(args)...));
  }
  ~CurrentScreenBlock() override;

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
   * @brief Create a new CurrentScreenBlock.
   * @param name defines the block name
   */
  CurrentScreenBlock(const std::string& name);

  /**
   * @brief Gets the uv input component.
   */
  NodeMaterialConnectionPointPtr& get_uv();

  /**
   * @brief Gets the rgba output component.
   */
  NodeMaterialConnectionPointPtr& get_rgba();

  /**
   * @brief Gets the rgb output component.
   */
  NodeMaterialConnectionPointPtr& get_rgb();

  /**
   * @brief Gets the r output component.
   */
  NodeMaterialConnectionPointPtr& get_r();

  /**
   * @brief Gets the g output component.
   */
  NodeMaterialConnectionPointPtr& get_g();

  /**
   * @brief Gets the b output component.
   */
  NodeMaterialConnectionPointPtr& get_b();

  /**
   * @brief Gets the a output component.
   */
  NodeMaterialConnectionPointPtr& get_a();

  /**
   * @brief Gets the target of the block.
   */
  NodeMaterialBlockTargets& get_target() override;

  /**
   * @brief Hidden
   */
  CurrentScreenBlock& _buildBlock(NodeMaterialBuildState& state) override;

private:
  void _injectVertexCode(NodeMaterialBuildState& state);
  void _writeTextureRead(NodeMaterialBuildState& state, bool vertexMode = false);
  void _writeOutput(NodeMaterialBuildState& state, const NodeMaterialConnectionPointPtr& output,
                    const std::string& swizzle, bool vertexMode = false);

public:
  /**
   * Gets or sets the texture associated with the node
   */
  BaseTexturePtr texture;

  /**
   * Gets or sets a boolean indicating if content needs to be converted to gamma space
   */
  bool convertToGammaSpace;

  /**
   * Gets or sets a boolean indicating if content needs to be converted to linear space
   */
  bool convertToLinearSpace;

  /**
   * Gets the uv input component
   */
  ReadOnlyProperty<CurrentScreenBlock, NodeMaterialConnectionPointPtr> uv;

  /**
   * Gets the rgba output component
   */
  ReadOnlyProperty<CurrentScreenBlock, NodeMaterialConnectionPointPtr> rgba;

  /**
   * Gets the rgb output component
   */
  ReadOnlyProperty<CurrentScreenBlock, NodeMaterialConnectionPointPtr> rgb;

  /**
   * Gets the r output component
   */
  ReadOnlyProperty<CurrentScreenBlock, NodeMaterialConnectionPointPtr> r;

  /**
   * Gets the g output component
   */
  ReadOnlyProperty<CurrentScreenBlock, NodeMaterialConnectionPointPtr> g;

  /**
   * Gets the b output component
   */
  ReadOnlyProperty<CurrentScreenBlock, NodeMaterialConnectionPointPtr> b;

  /**
   * Gets the a output component
   */
  ReadOnlyProperty<CurrentScreenBlock, NodeMaterialConnectionPointPtr> a;

private:
  std::string _samplerName;
  std::string _linearDefineName;
  std::string _gammaDefineName;
  std::string _mainUVName;
  std::string _tempTextureRead;
  NodeMaterialBlockTargets _currentTarget;

}; // end of class CurrentScreenBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DUAL_CURRENT_SHEEN_BLOCK_H
