#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DUAL_LIGHT_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DUAL_LIGHT_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class Light;
class LightBlock;
using LightPtr      = std::shared_ptr<Light>;
using LightBlockPtr = std::shared_ptr<LightBlock>;

/**
 * @brief Block used to add light in the fragment shader.
 */
class BABYLON_SHARED_EXPORT LightBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static LightBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<LightBlock>(new LightBlock(std::forward<Ts>(args)...));
  }
  ~LightBlock() override;

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
   * @brief Add uniforms, samplers and uniform buffers at compilation time.
   * @param state defines the state to update
   * @param nodeMaterial defines the node material requesting the update
   * @param defines defines the material defines to update
   * @param uniformBuffers defines the list of uniform buffer names
   */
  void updateUniformsAndSamples(NodeMaterialBuildState& state, const NodeMaterialPtr& nodeMaterial,
                                const NodeMaterialDefines& defines,
                                std::vector<std::string>& uniformBuffers) override;

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
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
   * @brief Creates a new LightBlock.
   * @param name defines the block name
   */
  LightBlock(const std::string& name);

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition();

  /**
   * @brief Gets the world normal input component.
   */
  NodeMaterialConnectionPointPtr& get_worldNormal();

  /**
   * @brief Gets the camera (or eye) position component.
   */
  NodeMaterialConnectionPointPtr& get_cameraPosition();

  /**
   * @brief Gets the glossiness component.
   */
  NodeMaterialConnectionPointPtr& get_glossiness();

  /**
   * @brief Gets the glossinness power component.
   */
  NodeMaterialConnectionPointPtr& get_glossPower();

  /**
   * @brief Gets the diffuse color component.
   */
  NodeMaterialConnectionPointPtr& get_diffuseColor();

  /**
   * @brief Gets the specular color component.
   */
  NodeMaterialConnectionPointPtr& get_specularColor();

  /**
   * @brief Gets the view matrix component.
   */
  NodeMaterialConnectionPointPtr& get_view();

  /**
   * @brief Gets the diffuse output component.
   */
  NodeMaterialConnectionPointPtr& get_diffuseOutput();

  /**
   * @brief Gets the specular output component.
   */
  NodeMaterialConnectionPointPtr& get_specularOutput();

  /**
   * @brief Gets the shadow output component.
   */
  NodeMaterialConnectionPointPtr& get_shadow();

  /**
   * @brief Hidden
   */
  LightBlock& _buildBlock(NodeMaterialBuildState& state) override;

private:
  void _injectVertexCode(NodeMaterialBuildState& state);

public:
  /**
   * Gets or sets the light associated with this block
   */
  LightPtr light;

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> worldPosition;

  /**
   * Gets the world normal input component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> worldNormal;

  /**
   * Gets the camera (or eye) position component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> cameraPosition;

  /**
   * Gets the glossiness component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> glossiness;

  /**
   * Gets the glossinness power component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> glossPower;

  /**
   * Gets the diffuse color component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> diffuseColor;

  /**
   * Gets the specular color component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> specularColor;

  /**
   * Gets the view matrix component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> view;

  /**
   * Gets the diffuse output component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> diffuseOutput;

  /**
   * Gets the specular output component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> specularOutput;

  /**
   * Gets the shadow output component
   */
  ReadOnlyProperty<LightBlock, NodeMaterialConnectionPointPtr> shadow;

private:
  uint32_t _lightId;

}; // end of class LightBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DUAL_Light_BLOCK_H
