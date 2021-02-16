#ifndef BABYLON_MATERIALS_TEXTURES_THIN_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_THIN_TEXTURE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/isize.h>

namespace BABYLON {

class ThinEngine;
FWD_CLASS_SPTR(InternalTexture)

/**
 * @brief Base class of all the textures in babylon.
 * It groups all the common properties required to work with Thin Engine.
 */
class BABYLON_SHARED_EXPORT ThinTexture {

public:
  /**
   * @brief Instantiates a new ThinTexture.
   * Base class of all the textures in babylon.
   * This can be used as an internal texture wrapper in ThinEngine to benefit from the cache
   * @param internalTexture Define the internalTexture to wrap
   */
  ThinTexture(const InternalTexturePtr& internalTexture);
  virtual ~ThinTexture(); // = default

  /**
   * @brief Get the class name of the texture.
   * @returns "ThinTexture"
   */
  std::string getClassName() const;

  /**
   * @brief Get if the texture is ready to be used (downloaded, converted, mip mapped...).
   * @returns true if fully ready
   */
  virtual bool isReady();

  /**
   * @brief Triggers the load sequence in delayed load mode.
   */
  virtual void delayLoad(const std::string& forcedExtension = "");

  /**
   * @brief Get the underlying lower level texture from Babylon.
   * @returns the internal texture
   */
  InternalTexturePtr& getInternalTexture();

  /**
   * @brief GGet the size of the texture.
   * @returns the texture size.
   */
  virtual ISize& getSize();

  /**
   * @brief Get the base size of the texture.
   * It can be different from the size if the texture has been resized for POT for instance
   * @returns the base size
   */
  virtual ISize& getBaseSize();

  // clang-format off
  /**
   * @brief Update the sampling mode of the texture.
   * Default is Trilinear mode.
   *
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 1     | NEAREST_SAMPLINGMODE or NEAREST_NEAREST_MIPLINEAR  | Nearest is: mag = nearest, min = nearest, mip = linear |
   * | 2     | BILINEAR_SAMPLINGMODE or LINEAR_LINEAR_MIPNEAREST | Bilinear is: mag = linear, min = linear, mip = nearest |
   * | 3     | TRILINEAR_SAMPLINGMODE or LINEAR_LINEAR_MIPLINEAR | Trilinear is: mag = linear, min = linear, mip = linear |
   * | 4     | NEAREST_NEAREST_MIPNEAREST |             |
   * | 5    | NEAREST_LINEAR_MIPNEAREST |             |
   * | 6    | NEAREST_LINEAR_MIPLINEAR |             |
   * | 7    | NEAREST_LINEAR |             |
   * | 8    | NEAREST_NEAREST |             |
   * | 9   | LINEAR_NEAREST_MIPNEAREST |             |
   * | 10   | LINEAR_NEAREST_MIPLINEAR |             |
   * | 11   | LINEAR_LINEAR |             |
   * | 12   | LINEAR_NEAREST |             |
   *
   *    > _mag_: magnification filter (close to the viewer)
   *    > _min_: minification filter (far from the viewer)
   *    > _mip_: filter used between mip map levels
   *@param samplingMode Define the new sampling mode of the texture
   */
  // clang-format on
  void updateSamplingMode(unsigned int samplingMode);

  /**
   * @brief Release and destroy the underlying lower level texture aka internalTexture.
   */
  void releaseInternalTexture();

  /**
   * @brief Dispose the texture and release its associated resources.
   */
  virtual void dispose();

protected:
  virtual unsigned int get_wrapU() const;
  virtual void set_wrapU(unsigned int value);
  virtual unsigned int get_wrapV() const;
  virtual void set_wrapV(unsigned int value);
  virtual unsigned int get_coordinatesMode() const;
  virtual void set_coordinatesMode(unsigned int value);
  virtual bool get_isCube() const;
  virtual void set_isCube(bool value);
  virtual bool get_is3D() const;
  virtual void set_is3D(bool value);
  virtual bool get_is2DArray() const;
  virtual void set_is2DArray(bool value);

public:
  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  Property<ThinTexture, unsigned int> wrapU;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  Property<ThinTexture, unsigned int> wrapV;

  /**
   * | Value | Type               | Description |
   * | ----- | ------------------ | ----------- |
   * | 0     | CLAMP_ADDRESSMODE  |             |
   * | 1     | WRAP_ADDRESSMODE   |             |
   * | 2     | MIRROR_ADDRESSMODE |             |
   */
  unsigned int wrapR;

  /**
   * With compliant hardware and browser (supporting anisotropic filtering)
   * this defines the level of anisotropic filtering in the texture.
   * The higher the better but the slower. This defaults to 4 as it seems to be the best tradeoff.
   */
  unsigned int anisotropicFilteringLevel;

  /**
   * Define the current state of the loading sequence when in delayed load mode.
   */
  unsigned int delayLoadState;

  /**
   * How a texture is mapped.
   * Unused in thin texture mode.
   */
  Property<ThinTexture, unsigned int> coordinatesMode;

  /**
   * Define if the texture is a cube texture or if false a 2d texture.
   */
  Property<ThinTexture, bool> isCube;

  /**
   * Define if the texture is a 3d texture (webgl 2) or if false a 2d texture.
   */
  Property<ThinTexture, bool> is3D;

  /**
   * Define if the texture is a 2d array texture (webgl 2) or if false a 2d texture.
   */
  Property<ThinTexture, bool> is2DArray;

  /**
   * Hidden
   */
  InternalTexturePtr _texture;

protected:
  unsigned int _wrapU;
  unsigned int _wrapV;
  ThinEngine* _engine;

private:
  ISize _cachedSize;
  ISize _cachedBaseSize;

}; // end of class ThinTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_THIN_TEXTURE_H
