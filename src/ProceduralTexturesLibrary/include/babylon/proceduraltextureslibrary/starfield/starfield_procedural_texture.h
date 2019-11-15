#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_STARFIELD_STARFIELD_PROCEDURAL_TEXTURE_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_STARFIELD_STARFIELD_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/procedurals/procedural_texture.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace ProceduralTexturesLibrary {

class StarfieldProceduralTexture;
using StarfieldProceduralTexturePtr
  = std::shared_ptr<StarfieldProceduralTexture>;

class BABYLON_SHARED_EXPORT StarfieldProceduralTexture
    : public ProceduralTexture {

public:
  template <typename... Ts>
  static StarfieldProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<StarfieldProceduralTexture>(
      new StarfieldProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~StarfieldProceduralTexture(); // = default

  void updateShaderUniforms();

  /**
   * @brief Serializes this starfield procedural texture.
   * @returns a serialized starfield procedural texture object
   */
  json serialize() const;

  /**
   * @brief Creates a Starfield Procedural Texture from parsed startfield
   * procedural texture data.
   * @param parsedTexture defines parsed texture data
   * @param scene defines the current scene
   * @param rootUrl defines the root URL containing startfield procedural
   * texture information
   * @returns a parsed Starfield Procedural Texture
   */
  static std::unique_ptr<StarfieldProceduralTexture>
  Parse(const json& parsedTexture, Scene* scene, const std::string& rootUrl);

protected:
  StarfieldProceduralTexture(const std::string& name, const Size& size,
                             Scene* scene, Texture* fallbackTexture = nullptr,
                             bool generateMipMaps = false);

  float get_time() const;
  void set_time(float value);
  float get_alpha() const;
  void set_alpha(float value);
  float get_beta() const;
  void set_beta(float value);
  float get_zoom() const;
  void set_zoom(float value);
  float get_formuparam() const;
  void set_formuparam(float value);
  float get_stepsize() const;
  void set_stepsize(float value);
  float get_tile() const;
  void set_tile(float value);
  float get_brightness() const;
  void set_brightness(float value);
  float get_darkmatter() const;
  void set_darkmatter(float value);
  float get_distfading() const;
  void set_distfading(float value);
  float get_saturation() const;
  void set_saturation(float value);

public:
  Property<StarfieldProceduralTexture, float> time;
  Property<StarfieldProceduralTexture, float> alpha;
  Property<StarfieldProceduralTexture, float> beta;
  Property<StarfieldProceduralTexture, float> zoom;
  Property<StarfieldProceduralTexture, float> formuparam;
  Property<StarfieldProceduralTexture, float> stepsize;
  Property<StarfieldProceduralTexture, float> tile;
  Property<StarfieldProceduralTexture, float> brightness;
  Property<StarfieldProceduralTexture, float> darkmatter;
  Property<StarfieldProceduralTexture, float> distfading;
  Property<StarfieldProceduralTexture, float> saturation;

private:
  float _time;
  float _alpha;
  float _beta;
  float _zoom;
  float _formuparam;
  float _stepsize;
  float _tile;
  float _brightness;
  float _darkmatter;
  float _distfading;
  float _saturation;

}; // end of class StarfieldProceduralTexture

} // end of namespace ProceduralTexturesLibrary
} // end of namespace BABYLON

#endif // end of
// BABYLON_PROCEDURAL_TEXTURES_LIBRARY_STARFIELD_STARFIELD_PROCEDURAL_TEXTURE_H
