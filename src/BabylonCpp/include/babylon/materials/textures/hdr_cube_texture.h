#ifndef BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

/**
 * This represents a texture coming from an HDR input.
 *
 * The only supported format is currently panorama picture stored in RGBE
 * format. Example of such files can be found on HDRLib: http://hdrlib.com/
 */
class BABYLON_SHARED_EXPORT HDRCubeTexture : public BaseTexture {

public:
  /**
   * Instantiates an HDRTexture from the following parameters.
   *
   * @param url The location of the HDR raw data (Panorama stored in RGBE
   * format)
   * @param scene The scene the texture will be used in
   * @param size The cubemap desired size (the more it increases the longer the
   * generation will be) If the size is omitted this implies you are using a
   * preprocessed cubemap.
   * @param noMipmap Forces to not generate the mipmap if true
   * @param generateHarmonics Specifies wether you want to extract the
   * polynomial harmonics during the generation process
   * @param useInGammaSpace Specifies if the texture will be use in gamma or
   * linear space (the PBR material requires those texture in linear space, but
   * the standard material would require them in Gamma space)
   * @param usePMREMGenerator Specifies wether or not to generate the CubeMap
   * through CubeMapGen to avoid seams issue at run time.
   */
  HDRCubeTexture(const std::string& url, Scene* scene, size_t size,
                 bool noMipmap = false, bool generateHarmonics = true,
                 bool useInGammaSpace = false, bool usePMREMGenerator = false);
  ~HDRCubeTexture();

  HDRCubeTexture* clone();

  /** Methods **/
  void delayLoad();
  Matrix* getReflectionTextureMatrix();

private:
  /**
   * Occurs when the file is a preprocessed .babylon.hdr file.
   */
  Float32Array loadBabylonTexture();

  /**
   * Occurs when the file is raw .hdr file.
   */
  Float32Array loadHDRTexture();

  /**
   * Starts the loading process of the texture.
   */
  void loadTexture();

public:
  /**
   * The texture URL.
   */
  std::string url;

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
   * Specifies wether the texture has been generated through the PMREMGenerator
   * tool.
   * This is usefull at run time to apply the good shader.
   */
  bool isPMREM;

public:
  static std::vector<std::string> _facesMapping;
  bool _useInGammaSpace;
  bool _generateHarmonics;
  bool _noMipmap;
  std::vector<std::string> _extensions;
  Matrix _textureMatrix;
  size_t _size;
  bool _usePMREMGenerator;
  bool _isBABYLONPreprocessed;

}; // end of class HDRCubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_HDR_CUBE_TEXTURE_H
