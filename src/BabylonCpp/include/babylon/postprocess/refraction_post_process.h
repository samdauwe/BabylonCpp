#ifndef BABYLON_POSTPROCESS_REFRACTION_POST_PROCESS_H
#define BABYLON_POSTPROCESS_REFRACTION_POST_PROCESS_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>
#include <babylon/postprocess/post_process.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT RefractionPostProcess : public PostProcess {

public:
  RefractionPostProcess(const std::string& name,
                        const std::string& refractionTextureUrl,
                        const Color3& color, float depth, int colorLevel,
                        float ratio, Camera* camera, unsigned int samplingMode,
                        Engine* engine, bool reusable = false);
  virtual ~RefractionPostProcess();

  void dispose(Camera* camera = nullptr) override;

public:
  Color3 color;
  float depth;
  int colorLevel;

private:
  Texture* _refRexture;

}; // end of class RefractionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_REFRACTION_POST_PROCESS_H
