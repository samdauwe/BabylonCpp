#ifndef BABYLON_HELPERS_PHOTO_DOME_H
#define BABYLON_HELPERS_PHOTO_DOME_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/transform_node.h>

namespace BABYLON {

class BackgroundMaterial;
class Mesh;
class PhotoDome;
struct PhotoDomeOptions;
class Texture;
using BackgroundMaterialPtr = std::shared_ptr<BackgroundMaterial>;
using PhotoDomePtr          = std::shared_ptr<PhotoDome>;
using MeshPtr               = std::shared_ptr<Mesh>;
using TexturePtr            = std::shared_ptr<Texture>;

/**
 * @brief Display a 360 degree photo on an approximately spherical surface,
 * useful for VR applications or skyboxes. As a subclass of TransformNode, this
 * allow parenting to the camera with different locations in the scene. This
 * class achieves its effect with a Texture and a correctly configured
 * BackgroundMaterial on an inverted sphere. Potential additions to this helper
 * include zoom and and non-infinite distance rendering effects.
 */
class BABYLON_SHARED_EXPORT PhotoDome : public TransformNode {

public:
  /**
   * Define the image as a Monoscopic panoramic 360 image.
   */
  static constexpr unsigned int MODE_MONOSCOPIC = 0;
  /**
   * Define the image as a Stereoscopic TopBottom/OverUnder panoramic 360 image.
   */
  static constexpr unsigned int MODE_TOPBOTTOM = 1;
  /**
   * Define the image as a Stereoscopic Side by Side panoramic 360 image.
   */
  static constexpr unsigned int MODE_SIDEBYSIDE = 2;

public:
  template <typename... Ts>
  static PhotoDomePtr New(Ts&&... args)
  {
    auto photoDome = std::shared_ptr<PhotoDome>(new PhotoDome(std::forward<Ts>(args)...));
    photoDome->addToScene(photoDome);

    return photoDome;
  }
  ~PhotoDome() override; // = default

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Create an instance of this class and pass through the parameters to
   * the relevant classes, Texture, StandardMaterial, and Mesh.
   * @param name Element's name, child elements will append suffixes for their
   * own names.
   * @param urlsOfPhoto defines the url of the photo to display
   * @param options defines an object containing optional or exposed sub element
   * properties
   * @param onError defines a callback called when an error occured while
   * loading the texture
   */
  PhotoDome(std::string name, const std::string& urlOfPhoto, PhotoDomeOptions options, Scene* scene,
            const std::function<void(const std::string& message)>& onError = nullptr);

  /**
   * @brief Gets the texture being displayed on the sphere.
   */
  TexturePtr& get_photoTexture();

  /**
   * @brief Sets the texture being displayed on the sphere.
   */
  void set_photoTexture(const TexturePtr& value);

  /**
   * @brief Gets the mesh used for the skybox.
   */
  MeshPtr& get_mesh();

  /**
   * @brief Gets the current fov(field of view) multiplier, 0.0 - 2.0. Defaults
   * to 1.0. Lower values "zoom in" and higher values "zoom out". Also see the
   * options.resolution property.
   */
  float get_fovMultiplier() const;

  /**
   * @brief Sets the current fov(field of view) multiplier, 0.0 - 2.0. Defaults
   * to 1.0. Lower values "zoom in" and higher values "zoom out". Also see the
   * options.resolution property.
   */
  void set_fovMultiplier(float value);

  /**
   * @brief Gets the current video mode for the video. It can be:
   * * PhotoDome.MODE_MONOSCOPIC : Define the image as a Monoscopic panoramic
   * 360 image.
   * * PhotoDome.MODE_TOPBOTTOM  : Define the image as a Stereoscopic
   * TopBottom/OverUnder panoramic 360 image.
   * * PhotoDome.MODE_SIDEBYSIDE : Define the image as a Stereoscopic Side by
   * Side panoramic 360 image.
   */
  unsigned int get_imageMode() const;

  /**
   * @brief Sets the current video mode for the video. It can be:
   * * PhotoDome.MODE_MONOSCOPIC : Define the image as a Monoscopic panoramic
   * 360 image.
   * * PhotoDome.MODE_TOPBOTTOM  : Define the image as a Stereoscopic
   * TopBottom/OverUnder panoramic 360 image.
   * * PhotoDome.MODE_SIDEBYSIDE : Define the image as a Stereoscopic Side by
   * Side panoramic 360 image.
   */
  void set_imageMode(unsigned int value);

private:
  void _changeImageMode(unsigned int value);

public:
  /**
   * Gets or sets the texture being displayed on the sphere
   */
  Property<PhotoDome, TexturePtr> photoTexture;

  /**
   * Observable raised when an error occured while loading the 360 image
   */
  Observable<std::string> onLoadErrorObservable;

  /**
   * Gets the mesh used for the skybox
   */
  ReadOnlyProperty<PhotoDome, MeshPtr> mesh;

  /**
   * The current fov(field of view) multiplier, 0.0 - 2.0. Defaults to 1.0.
   * Lower values "zoom in" and higher values "zoom out". Also see the
   * options.resolution property.
   */
  Property<PhotoDome, float> fovMultiplier;

  /**
   * Gets or sets the current video mode for the video. It can be:
   * * PhotoDome.MODE_MONOSCOPIC : Define the image as a Monoscopic panoramic
   * 360 image.
   * * PhotoDome.MODE_TOPBOTTOM  : Define the image as a Stereoscopic
   * TopBottom/OverUnder panoramic 360 image.
   * * PhotoDome.MODE_SIDEBYSIDE : Define the image as a Stereoscopic Side by
   * Side panoramic 360 image.
   */
  Property<PhotoDome, unsigned int> imageMode;

protected:
  /**
   * The texture being displayed on the sphere
   */
  TexturePtr _photoTexture;

  /**
   * The skybox material
   */
  BackgroundMaterialPtr _material;

  /**
   * The surface used for the skybox
   */
  MeshPtr _mesh;

private:
  bool _useDirectMapping;
  unsigned int _imageMode;
  Observer<Camera>::Ptr _onBeforeCameraRenderObserver;

}; // end of class PhotoDome

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_PHOTO_DOME_H
