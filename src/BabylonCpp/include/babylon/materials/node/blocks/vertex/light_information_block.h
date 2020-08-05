#ifndef BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_LIGHT_INFORMATION_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_LIGHT_INFORMATION_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class Light;
class LightInformationBlock;
using LightPtr                 = std::shared_ptr<Light>;
using LightInformationBlockPtr = std::shared_ptr<LightInformationBlock>;

/**
 * @brief Block used to get data information from a light.
 */
class BABYLON_SHARED_EXPORT LightInformationBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static LightInformationBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<LightInformationBlock>(
      new LightInformationBlock(std::forward<Ts>(args)...));
  }
  ~LightInformationBlock() override;

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
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
   * @brief Creates a new LightInformationBlock.
   * @param name defines the block name
   */
  LightInformationBlock(const std::string& iName);

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition();

  /**
   * @brief Gets the direction output component.
   */
  NodeMaterialConnectionPointPtr& get_direction();

  /**
   * @brief Gets the color output component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the intensity output component.
   */
  NodeMaterialConnectionPointPtr& get_intensity();

  /**
   * @brief Hidden
   */
  LightInformationBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets or sets the light associated with this block
   */
  LightPtr light;

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<LightInformationBlock, NodeMaterialConnectionPointPtr> worldPosition;

  /**
   * Gets the direction output component
   */
  ReadOnlyProperty<LightInformationBlock, NodeMaterialConnectionPointPtr> direction;

  /**
   * Gets the color output component
   */
  ReadOnlyProperty<LightInformationBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the intensity output component
   */
  ReadOnlyProperty<LightInformationBlock, NodeMaterialConnectionPointPtr> intensity;

private:
  std::string _lightDataUniformName;
  std::string _lightColorUniformName;
  std::string _lightTypeDefineName;

}; // end of class LightInformationBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_VERTEX_LIGHT_INFORMATION_BLOCK_H
