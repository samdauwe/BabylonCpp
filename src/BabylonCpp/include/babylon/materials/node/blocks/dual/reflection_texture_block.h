#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DUAL_REFLECTION_TEXTURE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DUAL_REFLECTION_TEXTURE_BLOCK_H

#include <string>

#include <babylon/babylon_api.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

class BaseTexture;
class ReflectionTextureBlock;
using BaseTexturePtr            = std::shared_ptr<BaseTexture>;
using ReflectionTextureBlockPtr = std::shared_ptr<ReflectionTextureBlock>;

/**
 * @brief Block used to read a reflection texture from a sampler.
 */
class BABYLON_SHARED_EXPORT ReflectionTextureBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static ReflectionTextureBlockPtr New(Ts&&... args)
  {
    return std::shared_ptr<ReflectionTextureBlock>(
      new ReflectionTextureBlock(std::forward<Ts>(args)...));
  }
  ~ReflectionTextureBlock() override;

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
                      NodeMaterialDefines& defines, bool useInstances = false) override;

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
  void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial,
            Mesh* mesh = nullptr) override;

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
  ReflectionTextureBlock(const std::string& name);

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_position();

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition();

  /**
   * @brief Gets the world normal input component.
   */
  NodeMaterialConnectionPointPtr& get_worldNormal();

  /**
   * @brief Gets the world input component.
   */
  NodeMaterialConnectionPointPtr& get_world();

  /**
   * @brief Gets the camera (or eye) position component.
   */
  NodeMaterialConnectionPointPtr& get_cameraPosition();

  /**
   * @brief Gets the view input component.
   */
  NodeMaterialConnectionPointPtr& get_view();

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
   * @brief Hidden
   */
  ReflectionTextureBlock& _buildBlock(NodeMaterialBuildState& state) override;

private:
  void _injectVertexCode(NodeMaterialBuildState& state);
  void _writeOutput(NodeMaterialBuildState& state, const NodeMaterialConnectionPointPtr& output,
                    const std::string& swizzle);

public:
  /**
   * Gets or sets the texture associated with the node
   */
  BaseTexturePtr texture;

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> position;

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> worldPosition;

  /**
   * Gets the world normal input component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> worldNormal;

  /**
   * Gets the world input component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> world;

  /**
   * Gets the camera (or eye) position component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> cameraPosition;

  /**
   * Gets the view input component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr&> view;

  /**
   * Gets the rgb output component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> rgb;

  /**
   *Gets the r output component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> r;

  /**
   * Gets the g output component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> g;

  /**
   * Gets the b output component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> b;

private:
  std::string _define3DName;
  std::string _defineCubicName;
  std::string _defineExplicitName;
  std::string _defineProjectionName;
  std::string _defineLocalCubicName;
  std::string _defineSphericalName;
  std::string _definePlanarName;
  std::string _defineEquirectangularName;
  std::string _defineMirroredEquirectangularFixedName;
  std::string _defineEquirectangularFixedName;
  std::string _defineSkyboxName;
  std::string _cubeSamplerName;
  std::string _2DSamplerName;
  std::string _positionUVWName;
  std::string _directionWName;
  std::string _reflectionCoordsName;
  std::string _reflection2DCoordsName;
  std::string _reflectionColorName;
  std::string _reflectionMatrixName;

}; // end of class ReflectionTextureBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DUAL_Light_BLOCK_H
