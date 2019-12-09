#ifndef BABYLON_POSTPROCESSES_COLOR_CORRECTION_POST_PROCESS_H
#define BABYLON_POSTPROCESSES_COLOR_CORRECTION_POST_PROCESS_H

#include <babylon/babylon_api.h>
#include <babylon/postprocesses/post_process.h>

namespace BABYLON {

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

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
  ColorCorrectionPostProcess(const std::string& name, const std::string& colorTableUrl, float ratio,
                             const CameraPtr& camera,
                             const std::optional<unsigned int>& samplingMode = std::nullopt,
                             Engine* engine = nullptr, bool reusable = false);
  ~ColorCorrectionPostProcess() override; // = default

private:
  TexturePtr _colorTableTexture;

}; // end of class ColorCorrectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESSES_COLOR_CORRECTION_POST_PROCESS_H
