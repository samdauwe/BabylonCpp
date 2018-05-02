#ifndef BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT CubeTexture : public BaseTexture {

public:
  CubeTexture(const string_t& rootUrl, Scene* scene,
              const vector_t<string_t>& extensions = {}, bool noMipmap = false,
              const vector_t<string_t>& files = {},
              const ::std::function<void(InternalTexture*, EventState&)>& onLoad
              = nullptr,
              const ::std::function<void()>& onError = nullptr,
              unsigned int format = EngineConstants::TEXTUREFORMAT_RGBA,
              bool prefiltered = false, const string_t& forcedExtension = "");
  ~CubeTexture() override;

  void delayLoad() override;
  Matrix* getReflectionTextureMatrix() override;
  void setReflectionTextureMatrix(const Matrix& value);
  unique_ptr_t<CubeTexture> clone() const;

  /** Static methods **/
  static unique_ptr_t<CubeTexture>
  CreateFromImages(const vector_t<string_t>& files, Scene* scene,
                   bool noMipmap = false);
  static unique_ptr_t<CubeTexture>
  CreateFromPrefilteredData(const string_t& url, Scene* scene,
                            const string_t& forcedExtension = "");
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

private:
  Nullable<Vector3> _boundingBoxSize;
  float _rotationY;
  bool _noMipmap;
  vector_t<string_t> _files;
  vector_t<string_t> _extensions;
  unique_ptr_t<Matrix> _textureMatrix;
  unsigned int _format;
  bool _prefiltered;

}; // end of class CubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_CUBE_TEXTURE_H
