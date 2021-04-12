#ifndef BABYLON_HELPERS_PHOTO_DOME_H
#define BABYLON_HELPERS_PHOTO_DOME_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/helpers/texture_dome.h>

namespace BABYLON {

FWD_CLASS_SPTR(PhotoDome)

/**
 * @brief Display a 360 degree photo on an approximately spherical surface, useful for VR
 * applications or skyboxes. As a subclass of TransformNode, this allow parenting to the camera with
 * different locations in the scene. This class achieves its effect with a Texture and a correctly
 * configured BackgroundMaterial on an inverted sphere. Potential additions to this helper include
 * zoom and and non-infinite distance rendering effects.
 */
class BABYLON_SHARED_EXPORT PhotoDome : public TextureDome {

public:
  /**
   * Define the image as a Monoscopic panoramic 360 image.
   */
  static constexpr unsigned int MODE_MONOSCOPIC = TextureDome::MODE_MONOSCOPIC;

  /**
   * Define the image as a Stereoscopic TopBottom/OverUnder panoramic 360 image.
   */
  static constexpr unsigned int MODE_TOPBOTTOM = TextureDome::MODE_TOPBOTTOM;

  /**
   * Define the image as a Stereoscopic Side by Side panoramic 360 image.
   */
  static constexpr unsigned int MODE_SIDEBYSIDE = TextureDome::MODE_SIDEBYSIDE;

public:
  template <typename... Ts>
  static PhotoDomePtr
  New(const std::string& name, const std::string& textureUrlOrElement, TextureDomeOptions& options,
      Scene* scene,
      const std::function<void(const std::string& message, const std::string& exception)>& onError
      = nullptr)
  {
    auto photoDome = std::shared_ptr<PhotoDome>(
      new PhotoDome(name, textureUrlOrElement, options, scene, onError));
    photoDome->initializeTextureDome(name, textureUrlOrElement, options, scene);
    photoDome->addToScene(photoDome);

    return photoDome;
  }
  ~PhotoDome() override; // = default

protected:
  /**
   * @brief Create an instance of this class and pass through the parameters to
   * the relevant classes, Texture, StandardMaterial, and Mesh.
   * @param name Element's name, child elements will append suffixes for their own names.
   * @param urlsOfPhoto defines the url of the photo to display
   * @param options defines an object containing optional or exposed sub element properties
   * @param onError defines a callback called when an error occured while loading the texture
   */
  PhotoDome(
    std::string name, const std::string& urlOfPhoto, TextureDomeOptions& options, Scene* scene,
    const std::function<void(const std::string& message, const std::string& exception)>& iOnError
    = nullptr);

  /**
   * @brief Gets the texture being displayed on the sphere.
   */
  TexturePtr& get_photoTexture();

  /**
   * @brief Sets the texture being displayed on the sphere.
   */
  void set_photoTexture(const TexturePtr& value);

  // clang-format off
  /**
   * @brief Gets the current video mode for the video. It can be:
   * * TextureDome.MODE_MONOSCOPIC : Define the texture source as a Monoscopic panoramic 360.
   * * TextureDome.MODE_TOPBOTTOM  : Define the texture source as a Stereoscopic TopBottom/OverUnder panoramic 360.
   * * TextureDome.MODE_SIDEBYSIDE : Define the texture source as a Stereoscopic Side by Side panoramic 360.
   */
  // clang-format on
  unsigned int get_imageMode() const;

  // clang-format off
  /**
   * @brief Sets the current video mode for the video. It can be:
   * * TextureDome.MODE_MONOSCOPIC : Define the texture source as a Monoscopic panoramic 360.
   * * TextureDome.MODE_TOPBOTTOM  : Define the texture source as a Stereoscopic TopBottom/OverUnder panoramic 360.
   * * TextureDome.MODE_SIDEBYSIDE : Define the texture source as a Stereoscopic Side by Side panoramic 360.
   */
  // clang-format on
  void set_imageMode(unsigned int value);

protected:
  /**
   * @brief Hidden
   */
  TexturePtr _initTexture(const std::string& urlsOrElement, Scene* scene,
                          const TextureDomeOptions& options) override;

public:
  /**
   * Gets or sets the texture being displayed on the sphere
   */
  Property<PhotoDome, TexturePtr> photoTexture;

  // clang-format off
  /**
   * @brief Gets or sets the current video mode for the video. It can be:
   * * TextureDome.MODE_MONOSCOPIC : Define the texture source as a Monoscopic panoramic 360.
   * * TextureDome.MODE_TOPBOTTOM  : Define the texture source as a Stereoscopic TopBottom/OverUnder panoramic 360.
   * * TextureDome.MODE_SIDEBYSIDE : Define the texture source as a Stereoscopic Side by Side panoramic 360.
   */
  // clang-format on
  Property<PhotoDome, unsigned int> imageMode;

}; // end of class PhotoDome

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_PHOTO_DOME_H
