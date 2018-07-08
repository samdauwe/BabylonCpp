#ifndef BABYLON_MATERIALS_TEXTURES_REFRACTION_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_REFRACTION_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/math/plane.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RefractionTexture : public RenderTargetTexture {

public:
  /**
   * @brief Creates a refraction texture used by refraction channel of the
   * standard material.
   * @param name the texture name
   * @param size size of the underlying texture
   * @param scene root scene
   */
  RefractionTexture(const string_t& name, ISize size, Scene* scene,
                    bool generateMipMaps = false);
  ~RefractionTexture();

  unique_ptr_t<RefractionTexture> clone();
  Json::object serialize() const;

public:
  Plane refractionPlane;
  float depth;

}; // end of class RefractionTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_REFRACTION_TEXTURE_H
