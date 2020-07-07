#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DUAL_TEXTURE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DUAL_TEXTURE_BLOCK_H

#include <string>

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class Texture;
class TextureBlock;
using TexturePtr      = std::shared_ptr<Texture>;
using TextureBlockPtr = std::shared_ptr<TextureBlock>;

/**
 * @brief Block used to read a texture from a sampler.
 */
class BABYLON_SHARED_EXPORT TextureBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static TextureBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<TextureBlock>(new TextureBlock(std::forward<Ts>(args)...));
  }
  ~TextureBlock() override;

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
   * @brief Initialize defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to be prepared
   * @param useInstances specifies that instances should be used
   */
  void initializeDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                         NodeMaterialDefines& defines, bool useInstances = false) override;

  /**
   * @brief Update defines for shader compilation.
   * @param mesh defines the mesh to be rendered
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param useInstances specifies that instances should be used
   */
  void prepareDefines(AbstractMesh* mesh, const NodeMaterialPtr& nodeMaterial,
                      NodeMaterialDefines& defines, bool useInstances = false,
                      const SubMeshPtr& subMesh = nullptr) override;

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
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

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
   * @brief Create a new TextureBlock.
   * @param name defines the block name
   */
  TextureBlock(const std::string& name);

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
   * @brief Gets the target.
   */
  NodeMaterialBlockTargets& get_target() override;

  /**
   * @brief Gets whether or not the target is a mixed block target.
   */
  bool get__isMixed() const;

  /**
   * @brief Hidden
   */
  TextureBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

private:
  void _injectVertexCode(NodeMaterialBuildState& state);
  void _writeTextureRead(NodeMaterialBuildState& state, bool vertexMode = false);
  void _writeOutput(NodeMaterialBuildState& state, const NodeMaterialConnectionPointPtr& output,
                    const std::string& swizzle, bool vertexMode = false);

public:
  /**
   * Gets or sets the texture associated with the node
   */
  TexturePtr texture;

  /**
   * Gets or sets a boolean indicating if content needs to be converted to gamma space
   */
  bool convertToGammaSpace;

  /**
   * Gets the uv input component.
   */
  ReadOnlyProperty<TextureBlock, NodeMaterialConnectionPointPtr> uv;

  /**
   * Gets the rgba output component.
   */
  ReadOnlyProperty<TextureBlock, NodeMaterialConnectionPointPtr> rgba;

  /**
   * Gets the rgb output component
   */
  ReadOnlyProperty<TextureBlock, NodeMaterialConnectionPointPtr> rgb;

  /**
   * Gets the r output component
   */
  ReadOnlyProperty<TextureBlock, NodeMaterialConnectionPointPtr> r;

  /**
   * Gets the g output component
   */
  ReadOnlyProperty<TextureBlock, NodeMaterialConnectionPointPtr> g;

  /**
   * Gets the b output component
   */
  ReadOnlyProperty<TextureBlock, NodeMaterialConnectionPointPtr> b;

  /**
   * Gets thea output component
   */
  ReadOnlyProperty<TextureBlock, NodeMaterialConnectionPointPtr> a;

  /**
   * Gets whether or not the target is a mixed block target
   */
  ReadOnlyProperty<TextureBlock, bool> _isMixed;

private:
  std::string _defineName;
  std::string _linearDefineName;
  std::string _tempTextureRead;
  std::string _samplerName;
  std::string _transformedUVName;
  std::string _textureTransformName;
  std::string _textureInfoName;
  std::string _mainUVName;
  std::string _mainUVDefineName;

  NodeMaterialBlockTargets _currentTarget;

}; // end of class TextureBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DUAL_Light_BLOCK_H
