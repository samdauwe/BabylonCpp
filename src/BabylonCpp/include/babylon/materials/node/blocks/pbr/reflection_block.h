#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFLECTION_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFLECTION_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/blocks/dual/reflection_texture_base_block.h>

namespace BABYLON {

class ReflectionBlock;
using ReflectionBlockPtr = std::shared_ptr<ReflectionBlock>;

/**
 * @brief Block used to implement the reflection module of the PBR material.
 */
class BABYLON_SHARED_EXPORT ReflectionBlock : public ReflectionTextureBaseBlock {

public:
  template <typename... Ts>
  static ReflectionBlockPtr New(Ts&&... args)
  {
    auto reflectionBlock
      = std::shared_ptr<ReflectionBlock>(new ReflectionBlock(std::forward<Ts>(args)...));
    ReflectionBlock::RegisterConnections(reflectionBlock);

    return reflectionBlock;
  }
  ~ReflectionBlock() override;

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const ReflectionBlockPtr& reflectionBlock);

  /**
   * @brief Gets the current class name.
   * @returns the class name
   */
  std::string getClassName() const override;

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
   * @brief Bind data to effect. Will only be called for blocks with isBindable === true.
   * @param effect defines the effect to bind data to
   * @param nodeMaterial defines the hosting NodeMaterial
   * @param mesh defines the mesh that will be rendered
   */
  void bind(const EffectPtr& effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

  /**
   * @brief Gets the code to inject in the vertex shader.
   * @param state current state of the node material building
   * @returns the shader code
   */
  std::string handleVertexSide(NodeMaterialBuildState& state) override;

  /**
   * @brief Gets the main code of the block (fragment side).
   * @param state current state of the node material building
   * @param normalVarName name of the existing variable corresponding to the normal
   * @returns the shader code
   */
  std::string getCode(NodeMaterialBuildState& state, const std::string& normalVarName);

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

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
   * @brief Creates a new ReflectionBlock.
   * @param name defines the block name
   */
  ReflectionBlock(const std::string& name);

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_position() override;

  /**
   * @brief Gets the world position input component.
   */
  NodeMaterialConnectionPointPtr& get_worldPosition() override;

  /**
   * @brief Gets the world normal input component.
   */
  NodeMaterialConnectionPointPtr& get_worldNormal() override;

  /**
   * @brief Gets the world input component.
   */
  NodeMaterialConnectionPointPtr& get_world() override;

  /**
   * @brief Gets the camera (or eye) position component.
   */
  NodeMaterialConnectionPointPtr& get_cameraPosition() override;

  /**
   * @brief Gets the view input component.
   */
  NodeMaterialConnectionPointPtr& get_view() override;

  /**
   * @brief Gets the color input component.
   */
  NodeMaterialConnectionPointPtr& get_color();

  /**
   * @brief Gets the reflection object output component.
   */
  NodeMaterialConnectionPointPtr& get_reflection();

  /**
   * @brief Returns true if the block has a texture (either its own texture or the environment
   * texture from the scene, if set).
   */
  bool get_hasTexture() const;

  /**
   * @brief Gets the reflection color (either the name of the variable if the color input is
   * connected, else a default value).
   */
  std::string get_reflectionColor() const;

  /**
   * @brief Gets the texture.
   */
  BaseTexturePtr _getTexture() const override;

  /**
   * @brief Hidden
   */
  ReflectionBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /** @hidden */
  std::string _defineLODReflectionAlpha;
  /** @hidden */
  std::string _defineLinearSpecularReflection;
  /** @hidden */
  std::string _vReflectionMicrosurfaceInfosName;
  /** @hidden */
  std::string _vReflectionInfosName;

  /**
   * The three properties below are set by the main PBR block prior to calling methods of this
   * class. This is to avoid having to add them as inputs here whereas they are already inputs of
   * the main block, so already known. It's less burden on the user side in the editor part.
   */

  /** @hidden */
  NodeMaterialConnectionPointPtr worldPositionConnectionPoint;
  /** @hidden */
  NodeMaterialConnectionPointPtr worldNormalConnectionPoint;
  /** @hidden */
  NodeMaterialConnectionPointPtr cameraPositionConnectionPoint;

  /**
   * Defines if the material uses spherical harmonics vs spherical polynomials for the
   * diffuse part of the IBL.
   */
  // @editableInPropertyPage("Spherical Harmonics", PropertyTypeForEdition.Boolean, "ADVANCED", {
  // "notifiers": { "update": true }})
  bool useSphericalHarmonics;

  /**
   * Force the shader to compute irradiance in the fragment shader in order to take bump in account.
   */
  // @editableInPropertyPage("Force irradiance in fragment", PropertyTypeForEdition.Boolean,
  // "ADVANCED", { "notifiers": { "update": true }})
  bool forceIrradianceInFragment;

  /**
   * Gets the color input component
   */
  ReadOnlyProperty<ReflectionBlock, NodeMaterialConnectionPointPtr> color;

  /**
   * Gets the reflection object output component
   */
  ReadOnlyProperty<ReflectionBlock, NodeMaterialConnectionPointPtr> reflection;

  /**
   * Returns true if the block has a texture (either its own texture or the environment texture from
   * the scene, if set)
   */
  ReadOnlyProperty<ReflectionBlock, bool> hasTexture;

  /**
   * Gets the reflection color (either the name of the variable if the color input is connected,
   * else a default value)
   */
  ReadOnlyProperty<ReflectionBlock, std::string> reflectionColor;

private:
  std::string _vEnvironmentIrradianceName;
  Scene* _scene;

}; // end of class ReflectionBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFLECTION_BLOCK_H
