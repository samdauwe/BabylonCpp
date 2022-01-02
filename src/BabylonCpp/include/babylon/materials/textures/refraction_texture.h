#ifndef BABYLON_MATERIALS_TEXTURES_REFRACTION_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_REFRACTION_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/maths/plane.h>

namespace BABYLON {

/**
 * @brief Creates a refraction texture used by refraction channel of the
 * standard material. It is like a mirror but to see through a material.
 * @see https://doc.babylonjs.com/how_to/reflect#refraction
 */
class BABYLON_SHARED_EXPORT RefractionTexture : public RenderTargetTexture {

public:
  /**
   * @brief Creates a refraction texture used by refraction channel of the
   * standard material. It is like a mirror but to see through a material.
   * @see https://doc.babylonjs.com/how_to/reflect#refraction
   * @param name Define the texture name
   * @param size Define the size of the underlying texture
   * @param scene Define the scene the refraction belongs to
   * @param generateMipMaps Define if we need to generate mips level for the
   * refraction
   */
  RefractionTexture(const std::string& name, ISize size, Scene* scene,
                    bool generateMipMaps = false);
  ~RefractionTexture() override; // = default

  /**
   * @brief Clone the refraction texture.
   * @returns the cloned texture
   */
  std::unique_ptr<RefractionTexture> clone();

  /**
   * @brief Serialize the texture to a JSON representation you could use in
   * Parse later on
   * @returns the serialized JSON representation
   */
  json serialize() const;

public:
  /**
   * Define the reflection plane we want to use. The refractionPlane is usually
   * set to the constructed refractor. It is possible to directly set the
   * refractionPlane by directly using a Plane(a, b, c, d) where a, b and c give
   * the plane normal vector (a, b, c) and d is a scalar displacement from the
   * refractionPlane to the origin. However in all but the very simplest of
   * situations it is more straight forward to set it to the refractor as stated
   * in the doc.
   * @see https://doc.babylonjs.com/how_to/reflect#refraction
   */
  Plane refractionPlane;

  /**
   * Define how deep under the surface we should see.
   */
  float depth;

}; // end of class RefractionTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_REFRACTION_TEXTURE_H
