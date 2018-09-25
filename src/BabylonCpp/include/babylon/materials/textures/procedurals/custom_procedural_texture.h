#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_CUSTOM_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_CUSTOM_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/core/json.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CustomProceduralTexture : public ProceduralTexture {

public:
  template <typename... Ts>
  static CustomProceduralTexture* New(Ts&&... args)
  {
    auto texture = new CustomProceduralTexture(std::forward<Ts>(args)...);
    texture->addToScene(
      static_cast<std::unique_ptr<CustomProceduralTexture>>(texture));

    return texture;
  }
  ~CustomProceduralTexture() override;

  bool isReady() override;
  void render(bool useCameraPostProcess = false) override;
  void updateTextures();
  void updateShaderUniforms();
  bool animate() const;
  void setAnimate(bool value);

protected:
  CustomProceduralTexture(const std::string& name,
                          const std::string& texturePath, int size,
                          Scene* scene, Texture* fallbackTexture = nullptr,
                          bool generateMipMaps = false);

private:
  void loadJson(const std::string& jsonUrl);

private:
  bool _animate;
  float _time;
  bool _configSet;
  Json::value _config;
  std::string _texturePath;

}; // end of class CustomProceduralTexture

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_CUSTOM_PROCEDURAL_TEXTURE_H
