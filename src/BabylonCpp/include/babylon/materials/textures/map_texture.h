#ifndef BABYLON_MATERIALS_TEXTURES_MAP_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_MAP_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/tools/rect_packing_map.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT MapTexture : public Texture {

public:
  MapTexture(const string_t& name, Scene* scene, const ISize& size,
             unsigned int samplingMode
             = TextureConstants::TRILINEAR_SAMPLINGMODE,
             bool useMipMap = false, float margin = 0.f);
  ~MapTexture();

  /**
   * @brief Allocate a rectangle of a given size in the texture map
   * @param size the size of the rectangle to allocation
   * @return the PackedRect instance corresponding to the allocated rect or null
   * is there was not enough space to allocate it.
   */
  PackedRect* allocateRect(const Size& size);

  /**
   * @brief Free a given rectangle from the texture map
   * @param rectInfo the instance corresponding to the rect to free.
   */
  void freeRect(PackedRect* rectInfo);

  /**
   * @brief Return the available space in the range of [O;1]. 0 being not space
   * left at all, 1 being an empty texture map.
   * This is the cumulated space, not the biggest available surface. Due to
   * fragmentation you may not allocate a rect corresponding to this surface.
   * @returns {}
   */
  float freeSpace() const;

  /**
   * @brief Bind the texture to the rendering engine to render in the zone of a
   * given rectangle.
   * Use this method when you want to render into the texture map with a
   * clipspace set to the location and size of the given rect.
   * Don't forget to call unbindTexture when you're done rendering
   * @param rect the zone to render to
   * @param clear true to clear the portion's color/depth data
   */
  void bindTextureForRect(const PackedRect& rect, bool clear);

  /**
   * @brief Bind the texture to the rendering engine to render in the zone of
   * the given size at the given position.
   * Use this method when you want to render into the texture map with a
   * clipspace set to the location and size of the given rect.
   * Don't forget to call unbindTexture when you're done rendering
   * @param pos the position into the texture
   * @param size the portion to fit the clip space to
   * @param clear true to clear the portion's color/depth data
   */
  void bindTextureForPosSize(const Vector2& pos, const Size& size, bool clear);

  /**
   * @brief Unbind the texture map from the rendering engine.
   * Call this method when you're done rendering. A previous call to
   * bindTextureForRect has to be made.
   * @param dumpForDebug if set to true the content of the texture map will be
   * dumped to a picture file that will be sent to the internet browser.
   */
  void unbindTexture(bool dumpForDebug = false);

  bool canRescale() const;

  std::unique_ptr<MapTexture> clone() const;

private:
  std::unique_ptr<RectPackingMap> _rectPackingMap;
  ISize _size;
  std::unique_ptr<Viewport> _replacedViewport;

}; // end of class MapTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_MAP_TEXTURE_H
