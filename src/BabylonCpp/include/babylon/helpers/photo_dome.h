#ifndef BABYLON_HELPERS_PHOTO_DOME_H
#define BABYLON_HELPERS_PHOTO_DOME_H

#include <babylon/babylon_global.h>
#include <babylon/engine/node.h>

namespace BABYLON {

/**
 * @brief Display a 360 degree photo on an approximately spherical surface,
 * useful for VR applications or skyboxes. As a subclass of Node, this allow
 * parenting to the camera with different locations in the scene. This class
 * achieves its effect with a Texture and a correctly configured
 * BackgroundMaterial on an inverted sphere. Potential additions to this helper
 * include zoom and and non-infinite distance rendering effects.
 */
class BABYLON_SHARED_EXPORT PhotoDome : public Node {

public:
  /**
   * @brief Create an instance of this class and pass through the parameters to
   * the relevant classes, Texture, StandardMaterial, and Mesh.
   * @param name Element's name, child elements will append suffixes for their
   * own names.
   * @param urlsOfPhoto define the url of the photo to display
   * @param options An object containing optional or exposed sub element
   * properties
   */
  PhotoDome(string_t name, const string_t& urlOfPhoto, PhotoDomeOptions options,
            Scene* scene);
  ~PhotoDome() override;

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Gets the texture being displayed on the sphere.
   */
  TexturePtr& get_photoTexture();

  /**
   * @brief Sets the texture being displayed on the sphere.
   */
  void set_photoTexture(const TexturePtr& value);

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

public:
  /**
   * The texture being displayed on the sphere
   */
  Property<PhotoDome, TexturePtr> photoTexture;

  /**
   * The current fov(field of view) multiplier, 0.0 - 2.0. Defaults
   * to 1.0. Lower values "zoom in" and higher values "zoom out". Also see the
   * options.resolution property
   */
  Property<PhotoDome, float> fovMultiplier;

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

}; // end of class PhotoDome

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_PHOTO_DOME_H
