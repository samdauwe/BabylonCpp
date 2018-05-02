#ifndef BABYLON_MATERIALS_TEXTURES_DYNAMIC_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_DYNAMIC_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

struct DynamicTextureOptions {
  ICanvas* canvas = nullptr;
  int width       = 0;
  int height      = 0;
}; // end of struct struct DynamicTexture

/**
 * @brief A class extending {BABYLON.Texture} allowing drawing on a texture.
 * @see http://doc.babylonjs.com/how_to/dynamictexture
 */
class BABYLON_SHARED_EXPORT DynamicTexture : public Texture {

public:
  /**
   * @brief Creates a {BABYLON.DynamicTexture}.
   * @param name defines the name of the texture
   * @param options provides 3 alternatives for width and height of texture, a
   * canvas, object with width and height properties, number for both width and
   * height
   * @param scene defines the scene where you want the texture
   * @param generateMipMaps defines the use of MinMaps or not (default is false)
   * @param samplingMode defines the sampling mode to use (default is
   * BABYLON.Texture.TRILINEAR_SAMPLINGMODE)
   * @param format defines the texture format to use (default is
   * BABYLON.Engine.TEXTUREFORMAT_RGBA)
   */
  DynamicTexture(const string_t& name, const DynamicTextureOptions& options,
                 Scene* scene, bool generateMipMaps,
                 unsigned int samplingMode
                 = TextureConstants::TRILINEAR_SAMPLINGMODE,
                 unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);
  ~DynamicTexture() override;

  /**
   * @brief Scales the texture.
   * @param ratio the scale factor to apply to both width and height
   */
  void scale(float ratio) override;

  /**
   * @brief Resizes the texture.
   * @param width the new width
   * @param height the new height
   */
  void scaleTo(int width, int height);

  /**
   * @brief Gets the context of the canvas used by the texture.
   * @returns the canvas context of the dynamic texture
   */
  ICanvasRenderingContext2D* getContext();

  /**
   * @brief Clears the texture.
   */
  void clear();

  /**
   * Updates the texture.
   * @param invertY defines the direction for the Y axis (default is true - y
   * increases downwards)
   */
  void update(bool invertY = true);

  /**
   * @brief Draws text onto the texture.
   * @param text defines the text to be drawn
   * @param x defines the placement of the text from the left
   * @param y defines the placement of the text from the top when invertY is
   * true and from the bottom when false
   * @param font defines the font to be used with font-style, font-size,
   * font-name
   * @param color defines the color used for the text
   * @param clearColor defines the color for the canvas, use null to not
   * overwrite canvas
   * @param invertY defines the direction for the Y axis (default is true - y
   * increases downwards)
   * @param update defines whether texture is immediately update (default is
   * true)
   */
  void drawText(const string_t& text, int x, int y, const string_t& font,
                const string_t& color, const string_t& clearColor,
                bool invertY = true, bool update = true);

  /**
   * @brief Clones the texture.
   * @returns the clone of the texture.
   */
  unique_ptr_t<DynamicTexture> clone() const;

  void _rebuild() override;

protected:
  /**
   * @brief Gets the current state of canRescale.
   */
  bool get_canRescale() const;

private:
  void _recreate(const ISize& textureSize);

public:
  ReadOnlyProperty<DynamicTexture, bool> canRescale;

private:
  bool _generateMipMaps;
  ICanvas* _canvas;
  ICanvasRenderingContext2D* _context;
  Engine* _engine;

}; // end of class DynamicTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_DYNAMIC_TEXTURE_H
