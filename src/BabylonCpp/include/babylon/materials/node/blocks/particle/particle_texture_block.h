#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_TEXTURE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_TEXTURE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class BaseTexture;
class ParticleTextureBlock;
using BaseTexturePtr          = std::shared_ptr<BaseTexture>;
using ParticleTextureBlockPtr = std::shared_ptr<ParticleTextureBlock>;

/**
 * @brief Base block used for the particle texture.
 */
class BABYLON_SHARED_EXPORT ParticleTextureBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ParticleTextureBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ParticleTextureBlock>(
      new ParticleTextureBlock(std::forward<Ts>(args)...));
  }
  ~ParticleTextureBlock() override;

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
   * @brief Creates a new ParticleTextureBlock.
   * @param name defines the block name
   */
  ParticleTextureBlock(const std::string& name);

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
   * @brief Hidden
   */
  ParticleTextureBlock& _buildBlock(NodeMaterialBuildState& state) override;

private:
  void _writeOutput(NodeMaterialBuildState& state, const NodeMaterialConnectionPointPtr& output,
                    const std::string& swizzle);

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
  ReadOnlyProperty<ParticleTextureBlock, NodeMaterialConnectionPointPtr> uv;

  /**
   * Gets the rgba output component
   */
  ReadOnlyProperty<ParticleTextureBlock, NodeMaterialConnectionPointPtr> rgba;

  /**
   * Gets the rgb output component
   */
  ReadOnlyProperty<ParticleTextureBlock, NodeMaterialConnectionPointPtr> rgb;

  /**
   * Gets the r output component
   */
  ReadOnlyProperty<ParticleTextureBlock, NodeMaterialConnectionPointPtr> r;

  /**
   * Gets the g output component
   */
  ReadOnlyProperty<ParticleTextureBlock, NodeMaterialConnectionPointPtr> g;

  /**
   * Gets the b output component
   */
  ReadOnlyProperty<ParticleTextureBlock, NodeMaterialConnectionPointPtr> b;

  /**
   * Gets the a output component
   */
  ReadOnlyProperty<ParticleTextureBlock, NodeMaterialConnectionPointPtr> a;

private:
  std::string _samplerName;
  std::string _linearDefineName;
  std::string _gammaDefineName;
  std::string _tempTextureRead;

}; // end of class ParticleTextureBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PARTICLE_PARTICLE_TEXTURE_BLOCK_H
