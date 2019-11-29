#ifndef BABYLON_HELPERS_IENVIRONMENT_HELPER_OPTIONS_H
#define BABYLON_HELPERS_IENVIRONMENT_HELPER_OPTIONS_H

#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class BaseTexture;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;

/**
 * @brief Represents the different options available during the creation of a
 * Environment helper.
 *
 * This can control the default ground, skybox and image processing setup of
 * your scene.
 */
struct BABYLON_SHARED_EXPORT IEnvironmentHelperOptions {

  /**
   * Specifies wether or not to create a ground.
   * True by default.
   */
  bool createGround = true;

  /**
   * Specifies the ground size.
   * 15 by default.
   */
  float groundSize = 15.f;

  /**
   * The texture used on the ground for the main color.
   * Comes from the BabylonJS CDN by default.
   *
   * Remarks: Can be either a texture or a url.
   */
  std::variant<std::string, BaseTexturePtr> groundTexture;

  /**
   * The color mixed in the ground texture by default.
   * BabylonJS clearColor by default.
   */
  Color3 groundColor;

  /**
   * Specifies the ground opacity.
   * 1 by default.
   */
  float groundOpacity = 1.f;

  /**
   * Enables the ground to receive shadows.
   * True by default.
   */
  bool enableGroundShadow = true;

  /**
   * Helps preventing the shadow to be fully black on the ground.
   * 0.5 by default.
   */
  float groundShadowLevel = 0.5f;

  /**
   * Creates a mirror texture attach to the ground.
   * false by default.
   */
  bool enableGroundMirror = false;

  /**
   * Specifies the ground mirror size ratio.
   * 0.3 by default as the default kernel is 64.
   */
  float groundMirrorSizeRatio = 0.3f;

  /**
   * Specifies the ground mirror blur kernel size.
   * 64 by default.
   */
  float groundMirrorBlurKernel = 64.f;

  /**
   * Specifies the ground mirror visibility amount.
   * 1 by default
   */
  float groundMirrorAmount = 1.f;

  /**
   * Specifies the ground mirror reflectance weight.
   * This uses the standard weight of the background material to setup the
   * fresnel effect
   * of the mirror.
   * 1 by default.
   */
  float groundMirrorFresnelWeight = 1.f;

  /**
   * Specifies the ground mirror Falloff distance.
   * This can helps reducing the size of the reflection.
   * 0 by Default.
   */
  float groundMirrorFallOffDistance = 0.f;

  /**
   * Specifies the ground mirror texture type.
   * Unsigned Int by Default.
   */
  unsigned int groundMirrorTextureType;

  /**
   * Specifies a bias applied to the ground vertical position to prevent
   * z-fighting with the shown objects.
   */
  float groundYBias;

  /**
   * Specifies wether or not to create a skybox.
   * True by default.
   */
  bool createSkybox = true;

  /**
   * Specifies the skybox size.
   * 20 by default.
   */
  float skyboxSize = 20.f;

  /**
   * The texture used on the skybox for the main color.
   * Comes from the BabylonJS CDN by default.
   *
   * Remarks: Can be either a texture or a url.
   */
  std::variant<std::string, BaseTexturePtr> skyboxTexture;

  /**
   * The color mixed in the skybox texture by default.
   * BabylonJS clearColor by default.
   */
  Color3 skyboxColor;

  /**
   * The background rotation around the Y axis of the scene.
   * This helps aligning the key lights of your scene with the background.
   * 0 by default.
   */
  float backgroundYRotation = 0.f;

  /**
   * Compute automatically the size of the elements to best fit with the scene.
   */
  bool sizeAuto;

  /**
   * Default position of the rootMesh if autoSize is not true.
   */
  Vector3 rootPosition;

  /**
   * Sets up the image processing in the scene.
   * true by default.
   */
  bool setupImageProcessing = true;

  /**
   * The texture used as your environment texture in the scene.
   * Comes from the BabylonJS CDN by default and in use if setupImageProcessing
   * is true.
   *
   * Remarks: Can be either a texture or a url.
   */
  std::variant<std::string, BaseTexturePtr> environmentTexture;

  /**
   * The value of the exposure to apply to the scene.
   * 0.6 by default if setupImageProcessing is true.
   */
  float cameraExposure = 0.6f;

  /**
   * The value of the contrast to apply to the scene.
   * 1.6 by default if setupImageProcessing is true.
   */
  float cameraContrast = 1.6f;

  /**
   * Specifies wether or not tonemapping should be enabled in the scene.
   * true by default if setupImageProcessing is true.
   */
  bool toneMappingEnabled = true;

}; // end of class IEnvironmentHelperOptions

} // end of namespace BABYLON

#endif // end of BABYLON_HELPERS_IENVIRONMENT_HELPER_OPTIONS_H
