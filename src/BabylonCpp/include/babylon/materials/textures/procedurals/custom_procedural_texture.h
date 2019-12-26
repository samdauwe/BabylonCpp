#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_CUSTOM_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_CUSTOM_PROCEDURAL_TEXTURE_H

#include <nlohmann/json.hpp>

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

using json = nlohmann::json;

namespace BABYLON {

class CustomProceduralTexture;
using CustomProceduralTexturePtr = std::shared_ptr<CustomProceduralTexture>;

/**
 * @brief Procedural texturing is a way to programmatically create a texture. There are 2 types of
 * procedural textures: code-only, and code that references some classic 2D images, sometimes called
 * 'refMaps' or 'sampler' images. Custom Procedural textures are the easiest way to create your own
 * procedural in your application.
 * @see
 * http://doc.babylonjs.com/how_to/how_to_use_procedural_textures#creating-custom-procedural-textures
 */
class BABYLON_SHARED_EXPORT CustomProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static CustomProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<CustomProceduralTexture>(
      new CustomProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~CustomProceduralTexture() override; // = default

  /**
   * @brief Is the texture ready to be used ? (rendered at least once)
   * @returns true if ready, otherwise, false.
   */
  bool isReady() override;

  /**
   * @brief Render the texture to its associated render target.
   * @param useCameraPostProcess Define if camera post process should be applied to the texture
   */
  void render(bool useCameraPostProcess = false) override;

  /**
   * @brief Update the list of dependant textures samplers in the shader.
   */
  void updateTextures();

  /**
   *@brief Update the uniform values of the procedural texture in the shader.
   */
  void updateShaderUniforms();

protected:
  /**
   * @brief Instantiates a new Custom Procedural Texture.
   * Procedural texturing is a way to programmatically create a texture. There are 2 types of
   * procedural textures: code-only, and code that references some classic 2D images, sometimes
   * called 'refMaps' or 'sampler' images. Custom Procedural textures are the easiest way to create
   * your own procedural in your application.
   * @see
   * http://doc.babylonjs.com/how_to/how_to_use_procedural_textures#creating-custom-procedural-textures
   * @param name Define the name of the texture
   * @param texturePath Define the folder path containing all the cutom texture related files
   * (config, shaders...)
   * @param size Define the size of the texture to create
   * @param scene Define the scene the texture belongs to
   * @param fallbackTexture Define a fallback texture in case there were issues to create the custom
   * texture
   * @param generateMipMaps Define if the texture should creates mip maps or not
   */
  CustomProceduralTexture(const std::string& name, const std::string& texturePath, int size,
                          Scene* scene, Texture* fallbackTexture = nullptr,
                          bool generateMipMaps = false);

  /**
   * @brief Gets if the texture animates or not.
   */
  bool get_animate() const;

  /**
   * @brief sets if the texture animates or not.
   */
  void set_animate(bool value);

private:
  void _loadJson(const std::string& jsonUrl);

public:
  /**
   * Define if the texture animates or not.
   */
  Property<CustomProceduralTexture, bool> animate;

private:
  bool _animate;
  float _time;
  bool _configSet;
  json _config;
  std::string _texturePath;

}; // end of class CustomProceduralTexture

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_CUSTOM_PROCEDURAL_TEXTURE_H
