#ifndef BABYLON_MATERIALS_TEXTURES_COLOR_GRADING_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_COLOR_GRADING_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class Engine;

/**
 * @brief This represents a color grading texture. This acts as a lookup table LUT, useful during
 * post process It can help converting any input color in a desired output one. This can then be
 * used to create effects from sepia, black and white to sixties or futuristic rendering...
 *
 * The only supported format is currently 3dl.
 * More information on LUT: https://en.wikipedia.org/wiki/3D_lookup_table
 */
class BABYLON_SHARED_EXPORT ColorGradingTexture : public BaseTexture {

public:
  /**
   * @brief Instantiates a ColorGradingTexture from the following parameters.
   *
   * @param url The location of the color gradind data (currently only supporting 3dl)
   * @param scene The scene the texture will be used in
   */
  ColorGradingTexture(const std::string& url, Scene* scene);
  ~ColorGradingTexture() override; // = default

  /**
   * @brief Returns the texture matrix used in most of the material.
   * This is not used in color grading but keep for troubleshooting purpose (easily swap diffuse by
   * colorgrading to look in).
   */
  Matrix* getTextureMatrix(int uBase = 1) override;

  /**
   * @brief Clones the color gradind texture.
   */
  std::unique_ptr<ColorGradingTexture> clone() const;

  /**
   * @brief Called during delayed load for textures.
   */
  void delayLoad(const std::string& forcedExtension = "") override;

  /**
   * @brief Parses a color grading texture serialized by Babylon.
   * @param parsedTexture The texture information being parsedTexture
   * @param scene The scene to load the texture in
   * @param rootUrl The root url of the data assets to load
   * @return A color gradind texture
   */
  static std::unique_ptr<ColorGradingTexture> Parse(const json& parsedTexture, Scene* scene,
                                                    const std::string& rootUrl);

  /**
   * @brief Serializes the LUT texture to json format.
   */
  json serialize() const;

private:
  /**
   * @brief Occurs when the file being loaded is a .3dl LUT file.
   */
  InternalTexturePtr load3dlTexture();

  /**
   * @brief Starts the loading process of the texture.
   */
  void loadTexture();

public:
  /**
   * The texture URL.
   */
  std::string url;

private:
  /**
   * The current texture matrix. (will always be identity in color grading texture)
   */
  std::unique_ptr<Matrix> _textureMatrix;

  Engine* _engine;

}; // end of class ColorGradingTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_COLOR_GRADING_TEXTURE_H
