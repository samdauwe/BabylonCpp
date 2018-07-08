#ifndef BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CubeTexture : public BaseTexture {

public:
  /**
   * Creates a cube texture to use with reflection for instance. It can be based
   * upon dds or six images as well as prefiltered data.
   * @param rootUrl defines the url of the texture or the root name of the six
   * images
   * @param scene defines the scene the texture is attached to
   * @param extensions defines the suffixes add to the picture name in case six
   * images are in use like _px.jpg...
   * @param noMipmap defines if mipmaps should be created or not
   * @param files defines the six files to load for the different faces
   * @param onLoad defines a callback triggered at the end of the file load if
   * no errors occured
   * @param onError defines a callback triggered in case of error during load
   * @param format defines the internal format to use for the texture once
   * loaded
   * @param prefiltered defines whether or not the texture is created from
   * prefiltered data
   * @param forcedExtension defines the extensions to use (force a special type
   * of file to load) in case it is different from the file name
   * @param createPolynomials defines whether or not to create polynomial
   * harmonics from the texture data if necessary
   * @param lodScale defines the scale applied to environment texture. This
   * manages the range of LOD level used for IBL according to the roughness
   * @param lodOffset defines the offset applied to environment texture. This
   * manages first LOD level used for IBL according to the roughness
   * @return the cube texture
   */
  CubeTexture(const string_t& rootUrl, Scene* scene,
              const vector_t<string_t>& extensions = {}, bool noMipmap = false,
              const vector_t<string_t>& files = {},
              const ::std::function<void(InternalTexture*, EventState&)>& onLoad
              = nullptr,
              const ::std::function<void()>& onError = nullptr,
              unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA,
              bool prefiltered = false, const string_t& forcedExtension = "",
              bool createPolynomials = false, float lodScale = 0.8f,
              float lodOffset = 0.f);
  ~CubeTexture() override;

  void delayLoad() override;
  Matrix* getReflectionTextureMatrix() override;
  void setReflectionTextureMatrix(const Matrix& value);
  unique_ptr_t<CubeTexture> clone() const;

  /** Static methods **/
  static unique_ptr_t<CubeTexture>
  CreateFromImages(const vector_t<string_t>& files, Scene* scene,
                   bool noMipmap = false);

  /**
   * @brief Creates and return a texture created from prefilterd data by tools
   * like IBL Baker or Lys.
   * @param url defines the url of the prefiltered texture
   * @param scene defines the scene the texture is attached to
   * @param forcedExtension defines the extension of the file if different from
   * the url
   * @param createPolynomials defines whether or not to create polynomial
   * harmonics from the texture data if necessary
   * @return the prefiltered texture
   */
  static unique_ptr_t<CubeTexture>
  CreateFromPrefilteredData(const string_t& url, Scene* scene,
                            const string_t& forcedExtension = "",
                            bool createPolynomials          = true);

  static unique_ptr_t<CubeTexture> Parse(const Json::value& parsedTexture,
                                         Scene* scene, const string_t& rootUrl);

protected:
  /**
   * @brief Gets or sets the size of the bounding box associated with the cube
   * texture When defined, the cubemap will switch to local mode
   * @see
   * https://community.arm.com/graphics/b/blog/posts/reflections-based-on-local-cubemaps-in-unity
   * Example: https://www.babylonjs-playground.com/#RNASML
   */
  void set_boundingBoxSize(const Nullable<Vector3>& value) override;
  Nullable<Vector3>& get_boundingBoxSize() override;

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

public:
  string_t url;
  unsigned int coordinatesMode;

  /**
   * Gets or sets the center of the bounding box associated with the cube
   * texture It must define where the camera used to render the texture was set
   */
  Vector3 boundingBoxPosition;

  /**
   * Texture matrix rotation angle around Y axis in radians
   */
  Property<CubeTexture, float> rotationY;

  /** Hidden */
  bool _prefiltered;

private:
  Nullable<Vector3> _boundingBoxSize;
  float _rotationY;
  bool _noMipmap;
  vector_t<string_t> _files;
  vector_t<string_t> _extensions;
  unique_ptr_t<Matrix> _textureMatrix;
  unsigned int _format;
  bool _createPolynomials;

}; // end of class CubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
