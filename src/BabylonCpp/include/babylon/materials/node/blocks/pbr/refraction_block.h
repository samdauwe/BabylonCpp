#ifndef BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFRACTION_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFRACTION_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/node/node_material_block.h>

namespace BABYLON {

FWD_CLASS_SPTR(BaseTexture)
FWD_CLASS_SPTR(RefractionBlock)

/**
 * @brief Block used to implement the refraction part of the sub surface module of the PBR material.
 */
class BABYLON_SHARED_EXPORT RefractionBlock : public NodeMaterialBlock {

public:
  template <typename... Ts>
  static RefractionBlockPtr New(Ts&&... args)
  {
    auto refractionBlock
      = std::shared_ptr<RefractionBlock>(new RefractionBlock(std::forward<Ts>(args)...));
    RefractionBlock::RegisterConnections(refractionBlock);

    return refractionBlock;
  }
  ~RefractionBlock() override;

  /**
   * @brief Hidden
   */
  static void RegisterConnections(const RefractionBlockPtr& refractionBlock);

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
   * @brief Gets the main code of the block (fragment side)
   * @param state current state of the node material building
   * @returns the shader code
   */
  std::string getCode(NodeMaterialBuildState& state);

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
   * @brief Creates a new RefractionBlock.
   * @param name defines the block name
   */
  RefractionBlock(const std::string& name);

  /**
   * @brief Gets the intensity input component.
   */
  NodeMaterialConnectionPointPtr& get_intensity();

  /**
   * @brief Gets the tint at distance input component.
   */
  NodeMaterialConnectionPointPtr& get_tintAtDistance();

  /**
   * @brief Gets the view input component.
   */
  NodeMaterialConnectionPointPtr& get_view();

  /**
   * @brief Gets the refraction object output component.
   */
  NodeMaterialConnectionPointPtr& get_refraction();

  /**
   * @brief Returns true if the block has a texture.
   */
  bool get_hasTexture() const;

  /**
   * @brief Hidden
   */
  BaseTexturePtr _getTexture() const;

  /**
   * @brief Hidden
   */
  RefractionBlock& _buildBlock(NodeMaterialBuildState& state) override;

  /**
   * @brief Hidden
   */
  std::string _dumpPropertiesCode() override;

public:
  /** @hidden */
  std::string _define3DName;
  /** @hidden */
  std::string _refractionMatrixName;
  /** @hidden */
  std::string _defineLODRefractionAlpha;
  /** @hidden */
  std::string _defineLinearSpecularRefraction;
  /** @hidden */
  std::string _defineOppositeZ;
  /** @hidden */
  std::string _cubeSamplerName;
  /** @hidden */
  std::string _2DSamplerName;
  /** @hidden */
  std::string _vRefractionMicrosurfaceInfosName;
  /** @hidden */
  std::string _vRefractionInfosName;
  /** @hidden */
  std::string _vRefractionFilteringInfoName;

  /**
   * The properties below are set by the main PBR block prior to calling methods of this class.
   * This is to avoid having to add them as inputs here whereas they are already inputs of the main
   * block, so already known. It's less burden on the user side in the editor part.
   */

  /** @hidden */
  NodeMaterialConnectionPointPtr viewConnectionPoint;

  /** @hidden */
  NodeMaterialConnectionPointPtr indexOfRefractionConnectionPoint;

  /**
   * This parameters will make the material used its opacity to control how much it is refracting
   * against not. Materials half opaque for instance using refraction could benefit from this
   * control.
   */
  // @editableInPropertyPage("Link refraction to transparency", PropertyTypeForEdition.Boolean,
  // "ADVANCED", { "notifiers": { "update": true }})
  bool linkRefractionWithTransparency;

  /**
   * Controls if refraction needs to be inverted on Y. This could be useful for procedural texture.
   */
  // @editableInPropertyPage("Invert refraction Y", PropertyTypeForEdition.Boolean, "ADVANCED", {
  // "notifiers": { "update": true }})
  bool invertRefractionY;

  /**
   * Gets or sets the texture associated with the node
   */
  BaseTexturePtr texture;

  /**
   * Gets the intensity input component
   */
  ReadOnlyProperty<RefractionBlock, NodeMaterialConnectionPointPtr> intensity;

  /**
   * Gets the tint at distance input component
   */
  ReadOnlyProperty<RefractionBlock, NodeMaterialConnectionPointPtr> tintAtDistance;

  /**
   * Gets the view input component
   */
  ReadOnlyProperty<RefractionBlock, NodeMaterialConnectionPointPtr> view;

  /**
   * Gets the refraction object output component
   */
  ReadOnlyProperty<RefractionBlock, NodeMaterialConnectionPointPtr> refraction;

  /**
   * Returns true if the block has a texture
   */
  ReadOnlyProperty<RefractionBlock, bool> hasTexture;

private:
  Scene* _scene;

}; // end of class RefractionBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_PBR_REFRACTION_BLOCK_H
