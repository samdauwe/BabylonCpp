#ifndef BABYLON_MATERIALS_TEXTURES_EQUI_RECTANGULAR_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_EQUI_RECTANGULAR_CUBE_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class EquiRectangularCubeTexture;
using EquiRectangularCubeTexturePtr = std::shared_ptr<EquiRectangularCubeTexture>;

/**
 * @brief This represents a texture coming from an equirectangular image supported by the web
 * browser canvas.
 */
class BABYLON_SHARED_EXPORT EquiRectangularCubeTexture : public BaseTexture {

public:
  template <typename... Ts>
  static EquiRectangularCubeTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<EquiRectangularCubeTexture>(
      new EquiRectangularCubeTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~EquiRectangularCubeTexture() override; // = default

  /**
   * @brief Get the current class name of the texture useful for serialization or dynamic coding.
   * @returns "EquiRectangularCubeTexture"
   */
  std::string getClassName() const;

  /**
   * @brief Create a clone of the current EquiRectangularCubeTexture and return it.
   * @returns A clone of the current EquiRectangularCubeTexture.
   */
  EquiRectangularCubeTexturePtr clone() const;

protected:
  /**
   * @brief Instantiates an EquiRectangularCubeTexture from the following parameters.
   * @param url The location of the image
   * @param scene The scene the texture will be used in
   * @param size The cubemap desired size (the more it increases the longer the generation will be)
   * @param noMipmap Forces to not generate the mipmap if true
   * @param gammaSpace Specifies if the texture will be used in gamma or linear space
   * (the PBR material requires those textures in linear space, but the standard material would
   * require them in Gamma space)
   * @param onLoad — defines a callback called when texture is loaded
   * @param onError — defines a callback called if there is an error
   */
  EquiRectangularCubeTexture(
    const std::string& url, Scene* scene, size_t size, bool noMipmap = false,
    bool gammaSpace = true, const std::function<void()>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr);

private:
  /**
   * @brief Load the image data, by putting the image on a canvas and extracting its buffer.
   */
  void loadImage(
    const std::function<void()>& loadTextureCallback,
    const std::function<void(const std::string& message, const std::string& exception)>& onError);

  /**
   * @brief Convert the image buffer into a cubemap and create a CubeTexture.
   */
  void loadTexture();

  /**
   * @brief Convert the ArrayBuffer into a Float32Array and drop the transparency channel.
   * @param buffer The ArrayBuffer that should be converted.
   * @returns The buffer as Float32Array.
   */
  Float32Array getFloat32ArrayFromArrayBuffer(const ArrayBuffer& buffer);

public:
  /**
   * The URL to the image.
   */
  std::string url;

  /**
   * The texture coordinates mode. As this texture is stored in a cube format, please modify
   * carefully.
   */
  unsigned int coordinatesMode;

private:
  /**
   * The six faces of the cube.
   */
  static std::array<std::string, 6> _FacesMapping;

  bool _noMipmap;
  std::function<void()> _onLoad;
  std::function<void(const std::string& message, const std::string& exception)> _onError;

  /**
   * The size of the cubemap.
   */
  size_t _size;

  /**
   * The buffer of the image.
   */
  ArrayBuffer _buffer;

  /**
   * The width of the input image.
   */
  size_t _width;

  /**
   * The height of the input image.
   */
  size_t _height;

}; // end of class EquiRectangularCubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_EQUI_RECTANGULAR_CUBE_TEXTURE_H
