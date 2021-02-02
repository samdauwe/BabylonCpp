#ifndef BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

FWD_CLASS_SPTR(HDRCubeTexture)

/**
 * @brief This represents a texture coming from an HDR input.
 *
 * The only supported format is currently panorama picture stored in RGBE format.
 * Example of such files can be found on HDRLib: http://hdrlib.com/
 */
class BABYLON_SHARED_EXPORT HDRCubeTexture : public BaseTexture {

public:
  template <typename... Ts>
  static HDRCubeTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<HDRCubeTexture>(new HDRCubeTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~HDRCubeTexture() override; // = default

  /**
   * @brief Get the current class name of the texture useful for serialization or dynamic coding.
   * @returns "HDRCubeTexture"
   */
  std::string getClassName() const;

  HDRCubeTexturePtr clone() const;

  /** Methods **/
  void delayLoad(const std::string& forcedExtension = "") override;

  /**
   * @brief Get the texture reflection matrix used to rotate/transform the reflection.
   * @returns the reflection matrix
   */
  Matrix* getReflectionTextureMatrix() override;

  /**
   * @brief Set the texture reflection matrix used to rotate/transform the
   * reflection.
   * @param value Define the reflection matrix to set
   */
  void setReflectionTextureMatrix(Matrix& value);

  /**
   * @brief Parses a JSON representation of an HDR Texture in order to create the texture
   * @param parsedTexture Define the JSON representation
   * @param scene Define the scene the texture should be created in
   * @param rootUrl Define the root url in case we need to load relative dependencies
   * @returns the newly created texture after parsing
   */
  static HDRCubeTexture* Parse(const json& parsedTexture, Scene* scene, const std::string& rootUrl);

  json serialize() const;

protected:
  /**
   * @brief Instantiates an HDRTexture from the following parameters.
   *
   * @param url The location of the HDR raw data (Panorama stored in RGBE format)
   * @param sceneOrEngine The scene or engine the texture will be used in
   * @param size The cubemap desired size (the more it increases the longer the generation will be)
   * @param noMipmap Forces to not generate the mipmap if true
   * @param generateHarmonics Specifies whether you want to extract the polynomial harmonics during
   * the generation process
   * @param gammaSpace Specifies if the texture will be use in gamma or linear space (the PBR
   * material requires those texture in linear space, but the standard material would require them
   * in Gamma space)
   * @param prefilterOnLoad Prefilters HDR texture to allow use of this texture as a PBR reflection
   * texture.
   */
  HDRCubeTexture(
    const std::string& url, const std::optional<std::variant<Scene*, ThinEngine*>>& sceneOrEngine,
    size_t size, bool noMipmap = false, bool generateHarmonics = true, bool gammaSpace = false,
    bool prefilterOnLoad = false, const std::function<void()>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);

  /**
   * @brief Sets whether or not the texture is blocking during loading.
   */
  void set_isBlocking(bool value) override;

  /**
   * @brief Gets whether or not the texture is blocking during loading.
   */
  bool get_isBlocking() const override;

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
  void set_boundingBoxSize(const std::optional<Vector3>& value) override;

  /**
   * @brief Gets the size of the bounding box associated with the cube texture.
   * When defined, the cubemap will switch to local mode
   * @see
   * https://community.arm.com/graphics/b/blog/posts/reflections-based-on-local-cubemaps-in-unity
   * Example https://www.babylonjs-playground.com/#RNASML
   */
  std::optional<Vector3>& get_boundingBoxSize() override;

private:
  /**
   * @brief Occurs when the file is raw .hdr file.
   */
  void loadTexture();

public:
  /**
   * The texture URL.
   */
  std::string url;

  /**
   * Texture matrix rotation angle around Y axis in radians.
   */
  Property<HDRCubeTexture, float> rotationY;

  /**
   * Gets or sets the center of the bounding box associated with the cube texture It must define
   * where the camera used to render the texture was set
   */
  Vector3 boundingBoxPosition;

protected:
  bool _isBlocking;
  float _rotationY;

private:
  static std::vector<std::string> _facesMapping;
  bool _generateHarmonics;
  bool _noMipmap;
  bool _prefilterOnLoad;
  std::vector<std::string> _extensions;
  Matrix _textureMatrix;
  size_t _size;
  std::function<void()> _onLoad;
  std::function<void(const std::string& message, const std::string& exception)> _onError;
  std::optional<Vector3> _boundingBoxSize;

}; // end of class HDRCubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H
