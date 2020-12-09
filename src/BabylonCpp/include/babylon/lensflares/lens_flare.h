#ifndef BABYLON_LENSFLARES_LENS_FLARE_H
#define BABYLON_LENSFLARES_LENS_FLARE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class LensFlare;
class LensFlareSystem;
class Texture;
FWD_CLASS_SPTR(LensFlare)
FWD_CLASS_SPTR(LensFlareSystem)
FWD_CLASS_SPTR(Texture)

/**
 * @brief This represents one of the lens effect in a `lensFlareSystem`.
 * It controls one of the indiviual texture used in the effect.
 * @see https://doc.babylonjs.com/how_to/how_to_use_lens_flares
 */
class BABYLON_SHARED_EXPORT LensFlare {

public:
  template <typename... Ts>
  static LensFlarePtr New(Ts&&... args)
  {
    auto lensFlare = std::shared_ptr<LensFlare>(new LensFlare(std::forward<Ts>(args)...));
    lensFlare->addToLensFlareSystem(lensFlare);

    return lensFlare;
  }
  virtual ~LensFlare(); // = default

  /**
   * @brief Hidden
   */
  void addToLensFlareSystem(const LensFlarePtr& lensFlare);

  /**
   * @brief Creates a new Lens Flare.
   * This represents one of the lens effect in a `lensFlareSystem`.
   * It controls one of the indiviual texture used in the effect.
   * @see https://doc.babylonjs.com/how_to/how_to_use_lens_flares
   * @param size Define the size of the lens flare (a floating value between 0 and 1)
   * @param position Define the position of the lens flare in the system. (a floating value between
   * -1 and 1). A value of 0 is located on the emitter. A value greater than 0 is beyond the emitter
   * and a value lesser than 0 is behind.
   * @param color Define the lens color
   * @param imgUrl Define the lens texture url
   * @param system Define the `lensFlareSystem` this flare is part of
   * @returns The newly created Lens Flare
   */
  static LensFlarePtr AddFlare(float size, float position, const Color3& color,
                               const std::string& imgUrl, const LensFlareSystemPtr& system);

  /**
   * @brief Dispose and release the lens flare with its associated resources.
   */
  void dispose();

protected:
  /**
   * @brief Instantiates a new Lens Flare.
   * This represents one of the lens effect in a `lensFlareSystem`.
   * It controls one of the indiviual texture used in the effect.
   * @see https://doc.babylonjs.com/how_to/how_to_use_lens_flares
   * @param size Define the size of the lens flare in the system (a floating value between 0 and 1)
   * @param position Define the position of the lens flare in the system. (a floating value between
   * -1 and 1). A value of 0 is located on the emitter. A value greater than 0 is beyond the emitter
   * and a value lesser than 0 is behind.
   * @param color Define the lens color
   * @param imgUrl Define the lens texture url
   * @param system Define the `lensFlareSystem` this flare is part of
   */
  LensFlare(float size, float position, const Color3& color = Color3(1.f, 1.f, 1.f),
            const std::string& imgUrl = "", const LensFlareSystemPtr& system = nullptr);

public:
  /**
   * Define the lens color.
   */
  Color3 color;

  /**
   * Define the lens texture.
   */
  TexturePtr texture;

  /**
   * Define the alpha mode to render this particular lens.
   */
  unsigned int alphaMode;

  /**
   * Define the size of the lens flare in the system (a floating value between 0 and 1)
   */
  float size;

  /**
   * Define the position of the lens flare in the system. (a floating value between -1 and 1). A
   * value of 0 is located on the emitter. A value greater than 0 is beyond the emitter and a value
   * lesser than 0 is behind.
   */
  float position;

private:
  LensFlareSystemPtr _system;

}; // end of class LensFlare

} // end of namespace BABYLON

#endif // end of BABYLON_LENSFLARES_LENS_FLARE_H
