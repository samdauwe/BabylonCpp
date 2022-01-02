#ifndef BABYLON_HELPERS_TEXTURE_DOME_H
#define BABYLON_HELPERS_TEXTURE_DOME_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/meshes/transform_node.h>

namespace BABYLON {

struct TextureDomeOptions;
FWD_CLASS_SPTR(BackgroundMaterial)
FWD_CLASS_SPTR(Mesh)
FWD_CLASS_SPTR(Texture)

/**
 * @brief Display a 360/180 degree texture on an approximately spherical surface, useful for VR
 * applications or skyboxes. As a subclass of TransformNode, this allow parenting to the camera or
 * multiple textures with different locations in the scene. This class achieves its effect with a
 * Texture and a correctly configured BackgroundMaterial on an inverted sphere. Potential additions
 * to this helper include zoom and and non-infinite distance rendering effects.
 */
class BABYLON_SHARED_EXPORT TextureDome : public TransformNode {

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
  /**
   * @brief Create an instance of this class and pass through the parameters to the relevant
   * classes- Texture, StandardMaterial, and Mesh.
   * @param name Element's name, child elements will append suffixes for their own names.
   * @param textureUrlOrElement defines the url(s) or the (video) HTML element to use
   * @param options An object containing optional or exposed sub element properties
   */
  TextureDome(
    const std::string& name, const std::string& textureUrlOrElement, TextureDomeOptions& options,
    Scene* scene,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);
  ~TextureDome(); // = default

  /**
   * @brief Hidden
   */
  void initializeTextureDome(const std::string& name, const std::string& textureUrlOrElement,
                             TextureDomeOptions& options, Scene* scene);

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse into each children
   * by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced materials and textures
   * (false by default)
   */
  void dispose(bool doNotRecurse = false, bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Gets the texture being displayed on the sphere.
   */
  TexturePtr& get_texture();

  /**
   * @brief Sets the texture being displayed on the sphere.
   */
  void set_texture(const TexturePtr& newTexture);

  /**
   * @brief Gets the mesh used for the dome.
   */
  MeshPtr& get_mesh();

  /**
   * @brief Gets the current fov(field of view) multiplier, 0.0 - 2.0. Defaults to 1.0. Lower values
   * "zoom in" and higher values "zoom out". Also see the options.resolution property.
   */
  float get_fovMultiplier() const;

  /**
   * @brief Sets the current fov(field of view) multiplier, 0.0 - 2.0. Defaults to 1.0. Lower values
   * "zoom in" and higher values "zoom out". Also see the options.resolution property.
   */
  void set_fovMultiplier(float value);

  // clang-format off
  /**
   * @brief Gets the current texture mode for the texture. It can be:
   * * TextureDome.MODE_MONOSCOPIC : Define the texture source as a Monoscopic panoramic 360.
   * * TextureDome.MODE_TOPBOTTOM  : Define the texture source as a Stereoscopic TopBottom/OverUnder panoramic 360.
   * * TextureDome.MODE_SIDEBYSIDE : Define the texture source as a Stereoscopic Side by Side panoramic 360.
   */
  // clang-format on
  unsigned int get_textureMode() const;

  // clang-format off
  /**
   * @brief Sets the current texture mode for the texture. It can be:
   * * TextureDome.MODE_MONOSCOPIC : Define the texture source as a Monoscopic panoramic 360.
   * * TextureDome.MODE_TOPBOTTOM  : Define the texture source as a Stereoscopic TopBottom/OverUnder panoramic 360.
   * * TextureDome.MODE_SIDEBYSIDE : Define the texture source as a Stereoscopic Side by Side panoramic 360.
   */
  // clang-format on
  void set_textureMode(unsigned int value);

  /**
   * @brief Is it a 180 degrees dome (half dome) or 360 texture (full dome).
   */
  bool get_halfDome() const;

  /**
   * @brief Set the halfDome mode. If set, only the front (180 degrees) will be displayed and the
   * back will be blacked out.
   */
  void set_halfDome(bool enabled);

  /**
   * @brief Set the cross-eye mode. If set, images that can be seen when crossing eyes will render
   * correctly.
   */
  bool get_crossEye() const;

  /**
   * @brief Is it a cross-eye texture?
   */
  void set_crossEye(bool enabled);

  /**
   * @brief Gets the background material of this dome.
   */
  BackgroundMaterialPtr& get_material();

  /**
   * @brief Hidden
   */
  virtual TexturePtr _initTexture(const std::string& urlsOrElement, Scene* scene,
                                  const TextureDomeOptions& options);

private:
  void _changeTextureMode(unsigned int value);

public:
  /**
   * The texture being displayed on the sphere
   */
  Property<TextureDome, TexturePtr> texture;

  /**
   * Gets the mesh used for the dome
   */
  ReadOnlyProperty<TextureDome, MeshPtr> mesh;

  /**
   * The current fov(field of view) multiplier, 0.0 - 2.0. Defaults to 1.0. Lower values "zoom in"
   * and higher values "zoom out". Also see the options.resolution property.
   */
  Property<TextureDome, float> fovMultiplier;

  // clang-format off
  /**
   * Sets the current texture mode for the texture. It can be:
   * * TextureDome.MODE_MONOSCOPIC : Define the texture source as a Monoscopic panoramic 360.
   * * TextureDome.MODE_TOPBOTTOM  : Define the texture source as a Stereoscopic TopBottom/OverUnder panoramic 360.
   * * TextureDome.MODE_SIDEBYSIDE : Define the texture source as a Stereoscopic Side by Side panoramic 360.
   */
  // clang-format on
  Property<TextureDome, unsigned int> textureMode;

  /**
   * Is it a 180 degrees dome (half dome) or 360 texture (full dome)
   */
  Property<TextureDome, bool> halfDome;

  /**
   * Gets or sets the cross-eye mode. If set, images that can be seen when crossing eyes will render
   * correctly
   */
  Property<TextureDome, bool> crossEye;

  /**
   * The background material of this dome
   */
  ReadOnlyProperty<TextureDome, BackgroundMaterialPtr> material;

protected:
  /**
   * Hidden
   */
  bool _useDirectMapping;

  /**
   * The texture being displayed on the sphere
   */
  TexturePtr _texture;

  /**
   * The skybox material
   */
  BackgroundMaterialPtr _material;

  /**
   * The surface used for the dome
   */
  MeshPtr _mesh;

  /**
   * The current texture mode for the texture. It can be:
   * * TextureDome.MODE_MONOSCOPIC : Define the texture source as a Monoscopic panoramic 360.
   * * TextureDome.MODE_TOPBOTTOM  : Define the texture source as a Stereoscopic TopBottom/OverUnder
   * panoramic 360.
   * * TextureDome.MODE_SIDEBYSIDE : Define the texture source as a Stereoscopic Side by Side
   * panoramic 360.
   */
  unsigned int _textureMode;

  /**
   * Observable raised when an error occured while loading the 360 image
   */
  Observable<std::string> onLoadErrorObservable;

  /**
   * Hidden
   */
  std::function<void(const std::string& message, const std::string& exception)> onError;

private:
  bool _halfDome;
  bool _crossEye;

  /**
   * A mesh that will be used to mask the back of the dome in case it is a 180 degree movie.
   */
  MeshPtr _halfDomeMask;

  /**
   * Oberserver used in Stereoscopic VR Mode.
   */
  Observer<Camera>::Ptr _onBeforeCameraRenderObserver;

}; // end of class PhotoDome

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_TEXTURE_DOME_H
