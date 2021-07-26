#ifndef BABYLON_LIGHTS_LIGHT_CONSTANTS_H
#define BABYLON_LIGHTS_LIGHT_CONSTANTS_H

#include <babylon/babylon_api.h>

namespace BABYLON {

struct ISortableLight;

/**
 * @brief Defines the cross module constants used by lights to avoid circular dependencies.
 */
struct BABYLON_SHARED_EXPORT LightConstants {
  /**
   * Falloff Default: light is falling off following the material specification:
   * standard material is using standard falloff whereas pbr material can request special falloff
   * per materials.
   */
  static constexpr unsigned int FALLOFF_DEFAULT = 0;

  /**
   * Falloff Physical: light is falling off following the inverse squared distance law.
   */
  static constexpr unsigned int FALLOFF_PHYSICAL = 1;

  /**
   * Falloff gltf: light is falling off as described in the gltf moving to PBR document
   * to enhance interoperability with other engines.
   */
  static constexpr unsigned int FALLOFF_GLTF = 2;

  /**
   * Falloff Standard: light is falling off like in the standard material
   * to enhance interoperability with other materials.
   */
  static constexpr unsigned int FALLOFF_STANDARD = 3;

  // lightmapMode Consts
  /**
   * If every light affecting the material is in this lightmapMode,
   * material.lightmapTexture adds or multiplies
   * (depends on material.useLightmapAsShadowmap)
   * after every other light calculations.
   */
  static constexpr unsigned int LIGHTMAP_DEFAULT = 0;
  /**
   * material.lightmapTexture as only diffuse lighting from this light
   * adds only specular lighting from this light
   * adds dynamic shadows
   */
  static constexpr unsigned int LIGHTMAP_SPECULAR = 1;
  /**
   * material.lightmapTexture as only lighting
   * no light calculation from this light
   * only adds dynamic shadows from this light
   */
  static constexpr unsigned int LIGHTMAP_SHADOWSONLY = 2;

  // Intensity Mode Consts
  /**
   * Each light type uses the default quantity according to its type:
   *      point/spot lights use luminous intensity
   *      directional lights use illuminance
   */
  static constexpr unsigned int INTENSITYMODE_AUTOMATIC = 0;
  /**
   * lumen (lm)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINOUSPOWER = 1;
  /**
   * candela (lm/sr)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINOUSINTENSITY = 2;
  /**
   * lux (lm/m^2)
   */
  static constexpr unsigned int INTENSITYMODE_ILLUMINANCE = 3;
  /**
   * nit (cd/m^2)
   */
  static constexpr unsigned int INTENSITYMODE_LUMINANCE = 4;

  // Light types ids const.
  /**
   * Light type const id of the point light.
   */
  static constexpr unsigned int LIGHTTYPEID_POINTLIGHT = 0;
  /**
   * Light type const id of the directional light.
   */
  static constexpr unsigned int LIGHTTYPEID_DIRECTIONALLIGHT = 1;
  /**
   * Light type const id of the spot light.
   */
  static constexpr unsigned int LIGHTTYPEID_SPOTLIGHT = 2;
  /**
   * Light type const id of the hemispheric light.
   */
  static constexpr unsigned int LIGHTTYPEID_HEMISPHERICLIGHT = 3;

  /**
   * @brief Sort function to order lights for rendering.
   * @param a First Light object to compare to second.
   * @param b Second Light object to compare first.
   * @return -1 to reduce's a's index relative to be, 0 for no change, 1 to increase a's index
   * relative to b.
   */
  static int CompareLightsPriority(const ISortableLight& a, const ISortableLight& b);
}; // end of struct ISortableLight

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_LIGHT_CONSTANTS_H
