#ifndef BABYLON_MATERIALS_NODE_BLOCKS_DUAL_REFLECTION_TEXTURE_BLOCK_H
#define BABYLON_MATERIALS_NODE_BLOCKS_DUAL_REFLECTION_TEXTURE_BLOCK_H

#include <babylon/babylon_api.h>
#include <babylon/materials/node/blocks/dual/reflection_texture_base_block.h>

namespace BABYLON {

class BaseTexture;
class ReflectionTextureBlock;
using BaseTexturePtr            = std::shared_ptr<BaseTexture>;
using ReflectionTextureBlockPtr = std::shared_ptr<ReflectionTextureBlock>;

/**
 * @brief Block used to read a reflection texture from a sampler.
 */
class BABYLON_SHARED_EXPORT ReflectionTextureBlock : public ReflectionTextureBaseBlock {

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

protected:
  /**
   * @brief Create a new TextureBlock.
   * @param name defines the block name
   */
  ReflectionTextureBlock(const std::string& name);

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
   * @brief Gets the rgb output component.
   */
  NodeMaterialConnectionPointPtr& get_rgb();

  /**
   * @brief Gets the rgba output component.
   */
  NodeMaterialConnectionPointPtr& get_rgba();

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
  ReflectionTextureBlock& _buildBlock(NodeMaterialBuildState& state) override;

public:
  /**
   * Gets the rgb output component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> rgb;

  /**
   * Gets the rgba output component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> rgba;

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

  /**
   * Gets the a output component
   */
  ReadOnlyProperty<ReflectionTextureBlock, NodeMaterialConnectionPointPtr> a;

}; // end of class ReflectionTextureBlock

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_NODE_BLOCKS_DUAL_Light_BLOCK_H
