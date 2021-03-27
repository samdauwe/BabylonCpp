#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DUAL_REFLECTION_TEXTURE_BASE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DUAL_REFLECTION_TEXTURE_BASE_BLOCK_H

#include <string>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(BaseTexture)

/**
 * @brief Base block used to read a reflection texture from a sampler.
 */
class BABYLON_SHARED_EXPORT ReflectionTextureBaseBlock : public NodeMaterialBlock {

public:
  ~ReflectionTextureBaseBlock() override;

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
  void bind(Effect* effect, const NodeMaterialPtr& nodeMaterial, Mesh* mesh = nullptr,
            SubMesh* subMesh = nullptr) override;

  /**
   * @brief Gets the code to inject in the vertex shader.
   * @param state current state of the node material building
   * @returns the shader code
   */
  virtual std::string handleVertexSide(NodeMaterialBuildState& state);

  /**
   * @brief Handles the inits for the fragment code path.
   * @param state node material build state
   */
  void handleFragmentSideInits(NodeMaterialBuildState& state);

  /**
   * @brief Generates the reflection coords code for the fragment code path.
   * @param worldNormalVarName name of the world normal variable
   * @param worldPos name of the world position variable. If not provided, will use the world
   * position connected to this block
   * @param onlyReflectionVector if true, generates code only for the reflection vector computation,
   * not for the reflection coordinates
   * @returns the shader code
   */
  std::string handleFragmentSideCodeReflectionCoords(std::string worldNormalVarName,
                                                     std::string worldPos      = "",
                                                     bool onlyReflectionVector = false);

  /**
   * @brief Generates the reflection color code for the fragment code path.
   * @param lodVarName name of the lod variable
   * @param swizzleLookupTexture swizzle to use for the final color variable
   * @returns the shader code
   */
  std::string handleFragmentSideCodeReflectionColor(const std::string& lodVarName = "",
                                                    const std::string& swizzleLookupTexture
                                                    = ".rgb");

  /**
   * @brief Generates the code corresponding to the connected output points.
   * @param state node material build state
   * @param varName name of the variable to output
   * @returns the shader code
   */
  std::string writeOutputs(NodeMaterialBuildState& state, const std::string& varName);

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
  ReflectionTextureBaseBlock(const std::string& name);

  /**
   * @brief Gets the texture associated with the node.
   */
  BaseTexturePtr& get_texture();

  /**
   * @brief Sets the texture associated with the node.
   */
  void set_texture(const BaseTexturePtr& texture);

  /**
   * @brief Gets the world position input component.
   */
  virtual NodeMaterialConnectionPointPtr& get_position() = 0;

  /**
   * @brief Gets the world position input component.
   */
  virtual NodeMaterialConnectionPointPtr& get_worldPosition() = 0;

  /**
   * @brief Gets the world normal input component.
   */
  virtual NodeMaterialConnectionPointPtr& get_worldNormal() = 0;

  /**
   * @brief Gets the world input component.
   */
  virtual NodeMaterialConnectionPointPtr& get_world() = 0;

  /**
   * @brief Gets the camera (or eye) position component.
   */
  virtual NodeMaterialConnectionPointPtr& get_cameraPosition() = 0;

  /**
   * @brief Gets the view input component.
   */
  virtual NodeMaterialConnectionPointPtr& get_view() = 0;

  /**
   * @brief Gets the texture.
   */
  virtual BaseTexturePtr _getTexture() const;

  /**
   * @brief Hidden
   */
  ReflectionTextureBaseBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

private:
  void _injectVertexCode(NodeMaterialBuildState& state);
  void _writeOutput(NodeMaterialBuildState& state, const NodeMaterialConnectionPointPtr& output,
                    const std::string& swizzle);

public:
  /** @hidden */
  std::string _define3DName;
  /** @hidden */
  std::string _defineCubicName;
  /** @hidden */
  std::string _defineExplicitName;
  /** @hidden */
  std::string _defineProjectionName;
  /** @hidden */
  std::string _defineLocalCubicName;
  /** @hidden */
  std::string _defineSphericalName;
  /** @hidden */
  std::string _definePlanarName;
  /** @hidden */
  std::string _defineEquirectangularName;
  /** @hidden */
  std::string _defineMirroredEquirectangularFixedName;
  /** @hidden */
  std::string _defineEquirectangularFixedName;
  /** @hidden */
  std::string _defineSkyboxName;
  /** @hidden */
  std::string _defineOppositeZ;
  /** @hidden */
  std::string _cubeSamplerName;
  /** @hidden */
  std::string _2DSamplerName;
  /** @hidden */
  std::string _reflectionPositionName;
  /** @hidden */
  std::string _reflectionSizeName;
  /** @hidden */
  std::string _reflectionCoordsName;
  /** @hidden */
  std::string _reflectionMatrixName;

  /**
   * Gets or sets the texture associated with the node
   */
  Property<ReflectionTextureBaseBlock, BaseTexturePtr> texture;

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<ReflectionTextureBaseBlock, NodeMaterialConnectionPointPtr> position;

  /**
   * Gets the world position input component
   */
  ReadOnlyProperty<ReflectionTextureBaseBlock, NodeMaterialConnectionPointPtr> worldPosition;

  /**
   * Gets the world normal input component
   */
  ReadOnlyProperty<ReflectionTextureBaseBlock, NodeMaterialConnectionPointPtr> worldNormal;

  /**
   * Gets the world input component
   */
  ReadOnlyProperty<ReflectionTextureBaseBlock, NodeMaterialConnectionPointPtr> world;

  /**
   * Gets the camera (or eye) position component
   */
  ReadOnlyProperty<ReflectionTextureBaseBlock, NodeMaterialConnectionPointPtr> cameraPosition;

  /**
   * Gets the view input component
   */
  ReadOnlyProperty<ReflectionTextureBaseBlock, NodeMaterialConnectionPointPtr> view;

protected:
  std::string _positionUVWName;
  std::string _directionWName;
  std::string _reflectionVectorName;
  std::string _reflectionColorName;
  BaseTexturePtr _texture;

}; // end of class ReflectionTextureBaseBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DUAL_REFLECTION_TEXTURE_BASE_BLOCK_H
