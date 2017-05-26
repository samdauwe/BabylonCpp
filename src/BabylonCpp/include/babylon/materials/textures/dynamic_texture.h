#ifndef BABYLON_MATERIALS_TEXTURES_DYNAMIC_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_DYNAMIC_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

struct DynamicTextureOptions {
  ICanvas* canvas = nullptr;
  int width;
  int height;
}; // end of struct struct DynamicTexture

class BABYLON_SHARED_EXPORT DynamicTexture : public Texture {

public:
  DynamicTexture(const std::string& name, const DynamicTextureOptions& options,
                 Scene* scene, bool generateMipMaps,
                 unsigned int samplingMode
                 = TextureConstants::TRILINEAR_SAMPLINGMODE,
                 unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA);
  ~DynamicTexture();

  bool canRescale();
  void scale(float ratio);
  void scaleTo(int width, int height);
  ICanvasRenderingContext2D* getContext();
  void clear();
  void update(bool invertY = true);
  void drawText(const std::string& text, int x, int y, const std::string& font,
                const std::string& color, const std::string& clearColor,
                bool invertY = true, bool update = true);
  std::unique_ptr<DynamicTexture> clone() const;

private:
  void _recreate(const ISize& textureSize);

public:
  unsigned int _samplingMode;

private:
  bool _generateMipMaps;
  ICanvas* _canvas;
  ICanvasRenderingContext2D* _context;

}; // end of class DynamicTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_DYNAMIC_TEXTURE_H
