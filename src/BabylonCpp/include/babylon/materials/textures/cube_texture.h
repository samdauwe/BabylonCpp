#ifndef BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class CubeTexture;
struct CubeTextureData;
using CubeTexturePtr = std::shared_ptr<CubeTexture>;

/**
 * @brief Class for creating a cube texture.
 */
class BABYLON_SHARED_EXPORT CubeTexture : public BaseTexture {

public:
  template <typename... Ts>
  static CubeTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<CubeTexture>(new CubeTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~CubeTexture() override; // = default

  /**
   * @brief Get the current class name of the texture useful for serialization
   * or dynamic coding.
   * @returns "CubeTexture"
   */
  std::string getClassName() const;

  /**
   * @brief Update the url (and optional buffer) of this texture if url was null during
   * construction.
   * @param url the url of the texture
   * @param forcedExtension defines the extension to use
   * @param onLoad callback called when the texture is loaded  (defaults to null)
   * @param prefiltered Defines whether the updated texture is prefiltered or not
   */
  void updateURL(const std::string& url, const std::string& forcedExtension = "",
                 const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad
                 = nullptr,
                 bool prefiltered = false);

  /**
   * @brief Delays loading of the cube texture.
   */
  void delayLoad(const std::string& forcedExtension = "") override;

  /**
   * @brief Returns the reflection texture matrix.
   * @returns the reflection texture matrix
   */
  Matrix* getReflectionTextureMatrix() override;

  /**
   * @brief Sets the reflection texture matrix.
   * @param value Reflection texture matrix
   */
  void setReflectionTextureMatrix(Matrix value);

  /**
   * @brief Makes a clone, or deep copy, of the cube texture.
   * @returns a new cube texture
   */
  CubeTexturePtr clone() const;

  /**
   * @brief Creates a cube texture from an array of image urls.
   * @param files defines an array of image urls
   * @param scene defines the hosting scene
   * @param noMipmap specifies if mip maps are not used
   * @returns a cube texture
   */
  static CubeTexturePtr CreateFromImages(const std::vector<std::string>& files, Scene* scene,
                                         bool noMipmap = false);

  /**
   * @brief Creates and return a texture created from prefilterd data by tools like IBL Baker or
   * Lys.
   * @param url defines the url of the prefiltered texture
   * @param scene defines the scene the texture is attached to
   * @param forcedExtension defines the extension of the file if different from the url
   * @param createPolynomials defines whether or not to create polynomial harmonics from the texture
   * data if necessary
   * @return the prefiltered texture
   */
  static CubeTexturePtr CreateFromPrefilteredData(const std::string& url, Scene* scene,
                                                  const std::string& forcedExtension = "",
                                                  bool createPolynomials             = true);

  /**
   * @brief Parses text to create a cube texture.
   * @param parsedTexture define the serialized text to read from
   * @param scene defines the hosting scene
   * @param rootUrl defines the root url of the cube texture
   * @returns a cube texture
   */
  static CubeTexturePtr Parse(const json& parsedTexture, Scene* scene, const std::string& rootUrl);

protected:
  /**
   * @brief Creates a cube texture to use with reflection for instance. It can be based upon dds or
   * six images as well as prefiltered data.
   * @param rootUrl defines the url of the texture or the root name of the six images
   * @param null defines the scene or engine the texture is attached to
   * @param extensions defines the suffixes add to the picture name in case six images are in use
   * like _px.jpg...
   * @param noMipmap defines if mipmaps should be created or not
   * @param files defines the six files to load for the different faces in that order: px, py, pz,
   * nx, ny, nz
   * @param onLoad defines a callback triggered at the end of the file load if no errors occured
   * @param onError defines a callback triggered in case of error during load
   * @param format defines the internal format to use for the texture once loaded
   * @param prefiltered defines whether or not the texture is created from prefiltered data
   * @param forcedExtension defines the extensions to use (force a special type of file to load) in
   * case it is different from the file name
   * @param createPolynomials defines whether or not to create polynomial harmonics from the texture
   * data if necessary
   * @param lodScale defines the scale applied to environment texture. This manages the range of LOD
   * level used for IBL according to the roughness
   * @param lodOffset defines the offset applied to environment texture. This manages first LOD
   * level used for IBL according to the roughness
   * @return the cube texture
   */
  CubeTexture(
    const std::string& rootUrl, const std::variant<Scene*, ThinEngine*>& sceneOrEngine,
    const std::vector<std::string>& extensions = {}, bool noMipmap = false,
    std::vector<std::string> files                                                = {},
    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int format = Constants::TEXTUREFORMAT_RGBA, bool prefiltered = false,
    const std::string& forcedExtension = "", bool createPolynomials = false, float lodScale = 0.8f,
    float lodOffset = 0.f);

  /**
   * @brief Gets or sets the size of the bounding box associated with the cube
   * texture When defined, the cubemap will switch to local mode
   * @see
   * https://community.arm.com/graphics/b/blog/posts/reflections-based-on-local-cubemaps-in-unity
   * Example: https://www.babylonjs-playground.com/#RNASML
   */
  void set_boundingBoxSize(const std::optional<Vector3>& value) override;

  /**
   * @brief Returns the bounding box size.
   * @see http://doc.babylonjs.com/how_to/reflect#using-local-cubemap-mode
   */
  std::optional<Vector3>& get_boundingBoxSize() override;

  /**
   * @brief Sets texture matrix rotation angle around Y axis in radians.
   * @param value rotation angle around Y axis in radians
   */
  void set_rotationY(float value);

  /**
   * @brief Gets texture matrix rotation angle around Y axis radians.
   * @return rotation angle around Y axis radians
   */
  float get_rotationY() const;

  /**
   * @brief Are mip maps generated for this texture or not.
   */
  bool get_noMipmap() const override;

public:
  /**
   * Observable triggered once the texture has been loaded.
   */
  Observable<CubeTexture> onLoadObservable;

  /**
   * The url of the texture
   */
  std::string url;

  /**
   * Gets or sets the center of the bounding box associated with the cube
   * texture. It must define where the camera used to render the texture was set
   * @see http://doc.babylonjs.com/how_to/reflect#using-local-cubemap-mode
   */
  Vector3 boundingBoxPosition;

  /**
   * Texture matrix rotation angle around Y axis in radians
   */
  Property<CubeTexture, float> rotationY;

  /**
   * Are mip maps generated for this texture or not
   */
  ReadOnlyProperty<CubeTexture, bool> noMipmap;

protected:
  std::string _forcedExtension;

private:
  std::function<void()> _onLoadProcessing;
  std::function<void(const std::optional<CubeTextureData>& data)> _delayedOnLoad;
  std::optional<Vector3> _boundingBoxSize;
  float _rotationY;
  bool _noMipmap;
  std::vector<std::string> _files;
  std::vector<std::string> _extensions;
  std::unique_ptr<Matrix> _textureMatrix;
  unsigned int _format;
  bool _createPolynomials;
  float _lodScale;
  float _lodOffset;

}; // end of class CubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
