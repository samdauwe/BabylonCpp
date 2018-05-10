#ifndef BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

/**
 * @brief This represents a texture coming from an HDR input.
 *
 * The only supported format is currently panorama picture stored in RGBE
 * format. Example of such files can be found on HDRLib: http://hdrlib.com/
 */
class BABYLON_SHARED_EXPORT HDRCubeTexture : public BaseTexture {

public:
  /**
   * @brief Instantiates an HDRTexture from the following parameters.
   * @param url The location of the HDR raw data (Panorama stored in RGBE
   * format)
   * @param scene The scene the texture will be used in
   * @param size The cubemap desired size (the more it increases the longer the
   * generation will be)
   * @param noMipmap Forces to not generate the mipmap if true
   * @param generateHarmonics Specifies whether you want to extract the
   * polynomial harmonics during the generation process
   * @param gammaSpace Specifies if the texture will be use in gamma or linear
   * space (the PBR material requires those texture in linear space, but the
   * standard material would require them in Gamma space)
   * @param reserved Reserved flag for internal use.
   */
  HDRCubeTexture(const string_t& url, Scene* scene, size_t size,
                 bool noMipmap = false, bool generateHarmonics = true,
                 bool gammaSpace = false, bool reserved = false,
                 const ::std::function<void()>& onLoad = nullptr,
                 const ::std::function<void(const string_t& message,
                                            const string_t& exception)>& onError
                 = nullptr);
  ~HDRCubeTexture() override;

  HDRCubeTexture* clone();

  /** Methods **/
  void delayLoad() override;
  Matrix* getReflectionTextureMatrix() override;
  void setReflectionTextureMatrix(const Matrix& value);
  static HDRCubeTexture* Parse(const Json::value& parsedTexture, Scene* scene,
                               const string_t& rootUrl);
  Json::object serialize() const;

protected:
  /**
   * @brief Sets wether or not the texture is blocking during loading.
   */
  void set_isBlocking(bool value);

  /**
   * @brief Gets wether or not the texture is blocking during loading.
   */
  bool get_isBlocking() const;

  /**
   * @brief Sets texture matrix rotation angle around Y axis in radians.
   */
  void set_rotationY(float value);

  /**
   * @brief Gets texture matrix rotation angle around Y axis radians.
   */
  float get_rotationY() const;

  /**
   * @brief Sets the size of the bounding box associated with the cube texture.
   * When defined, the cubemap will switch to local mode
   * @see
   * https://community.arm.com/graphics/b/blog/posts/reflections-based-on-local-cubemaps-in-unity
   * Example https://www.babylonjs-playground.com/#RNASML
   */
  void set_boundingBoxSize(const Nullable<Vector3>& value) override;

  /**
   * @brief Gets the size of the bounding box associated with the cube texture.
   * When defined, the cubemap will switch to local mode
   * @see
   * https://community.arm.com/graphics/b/blog/posts/reflections-based-on-local-cubemaps-in-unity
   * Example https://www.babylonjs-playground.com/#RNASML
   */
  Nullable<Vector3>& get_boundingBoxSize() override;

private:
  /**
   * @brief Occurs when the file is raw .hdr file.
   */
  Float32Array loadTexture();

public:
  /**
   * The texture URL.
   */
  string_t url;

  /**
   * The texture coordinates mode. As this texture is stored in a cube format,
   * please modify carefully.
   */
  unsigned int coordinatesMode;

  /**
   * The spherical polynomial data extracted from the texture.
   */
  SphericalPolynomial* sphericalPolynomial;

  /**
   * Specifies wether or not the texture is blocking during loading.
   */
  Property<HDRCubeTexture, bool> isBlocking;

  /**
   * Texture matrix rotation angle around Y axis in radians.
   */
  Property<HDRCubeTexture, float> rotationY;

  /**
   * Gets or sets the center of the bounding box associated with the cube
   * texture It must define where the camera used to render the texture was set
   */
  Vector3 boundingBoxPosition;

protected:
  bool _isBlocking;
  float _rotationY;

private:
  static vector_t<string_t> _facesMapping;
  bool _generateHarmonics;
  bool _noMipmap;
  vector_t<string_t> _extensions;
  Matrix _textureMatrix;
  size_t _size;
  ::std::function<void()> _onLoad;
  ::std::function<void(const string_t& message, const string_t& exception)>
    _onError;
  Nullable<Vector3> _boundingBoxSize;

}; // end of class HDRCubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H
