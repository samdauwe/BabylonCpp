#ifndef BABYLON_POSTPROCESS_COLOR_CORRECTION_POST_PROCESS_H
#define BABYLON_POSTPROCESS_COLOR_CORRECTION_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief This post-process allows the modification of rendered colors by using
 * a 'look-up table' (LUT). This effect is also called Color Grading.
 *
 *  The object needs to be provided an url to a texture containing the color
 *  look-up table: the texture must be 256 pixels wide and 16 pixels high.
 *  Use an image editing software to tweak the LUT to match your needs.
 *
 *  For an example of a color LUT, see here:
 *      http://udn.epicgames.com/Three/rsrc/Three/ColorGrading/RGBTable16x1.png
 *  For explanations on color grading, see here:
 *      http://udn.epicgames.com/Three/ColorGrading.html
 */
class BABYLON_SHARED_EXPORT ColorCorrectionPostProcess : public PostProcess {

public:
  ColorCorrectionPostProcess(const std::string& name,
                             const std::string& colorTableUrl, float ratio,
                             Camera* camera, unsigned int samplingMode,
                             Engine* engine, bool reusable = false);
  ~ColorCorrectionPostProcess();

private:
  Texture* _colorTableTexture;

}; // end of class ColorCorrectionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_COLOR_CORRECTION_POST_PROCESS_H
