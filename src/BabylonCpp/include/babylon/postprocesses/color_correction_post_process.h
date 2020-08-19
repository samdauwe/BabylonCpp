#ifndef BABYLON_POSTPROCESSES_COLOR_CORRECTION_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_COLOR_CORRECTION_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class ColorCorrectionPostProcess;
class Texture;
using ColorCorrectionPostProcessPtr = std::shared_ptr<ColorCorrectionPostProcess>;
using TexturePtr                    = std::shared_ptr<Texture>;

/**
 * @brief This post-process allows the modification of rendered colors by using
 * a 'look-up table' (LUT). This effect is also called Color Grading.
 *
 * The object needs to be provided an url to a texture containing the color
 * look-up table: the texture must be 256 pixels wide and 16 pixels high.
 * Use an image editing software to tweak the LUT to match your needs.
 *
 * For an example of a color LUT, see here:
 * @see http://udn.epicgames.com/Three/rsrc/Three/ColorGrading/RGBTable16x1.png
 * For explanations on color grading, see here:
 * @see http://udn.epicgames.com/Three/ColorGrading.html
 *
 */
class BABYLON_SHARED_EXPORT ColorCorrectionPostProcess : public PostProcess {

public:
  ColorCorrectionPostProcess(const std::string& name, const std::string& colorTableUrl,
                             const std::variant<float, PostProcessOptions>& options,
                             const CameraPtr& camera,
                             const std::optional<unsigned int>& samplingMode = std::nullopt,
                             Engine* engine = nullptr, bool reusable = false);
  ~ColorCorrectionPostProcess() override; // = default

  /**
   * @brief Gets a string identifying the name of the class.
   * @returns "ColorCorrectionPostProcess" string
   */
  std::string getClassName() const override;

  /**
   * @brief Hidden
   */
  static ColorCorrectionPostProcessPtr _Parse(const json& parsedPostProcess,
                                              const CameraPtr& targetCamera, Scene* scene,
                                              const std::string& rootUrl);

public:
  /**
   * Gets the color table url used to create the LUT texture
   */
  std::string colorTableUrl;

private:
  TexturePtr _colorTableTexture;

}; // end of class ColorCorrectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_COLOR_CORRECTION_POST_PROCESS_H
