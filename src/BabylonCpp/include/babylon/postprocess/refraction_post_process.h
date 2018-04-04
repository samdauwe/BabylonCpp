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
  RefractionPostProcess(const string_t& name,
                        const string_t& refractionTextureUrl,
                        const Color3& color, float depth, int colorLevel,
                        float ratio, Camera* camera, unsigned int samplingMode,
                        Engine* engine, bool reusable = false);
  virtual ~RefractionPostProcess() override;

  void dispose(Camera* camera = nullptr) override;

private:
  /**
   * @brief Gets the refraction texture.
   * Please note that you are responsible for disposing the texture if you set
   * it manually
   */
  Texture*& get_refractionTexture();

  /**
   * @brief Sets the refraction texture.
   * Please note that you are responsible for disposing the texture if you set
   * it manually
   */
  void set_refractionTexture(Texture* const& value);

public:
  Color3 color;
  float depth;
  int colorLevel;

  Property<RefractionPostProcess, Texture*> refractionTexture;

private:
  Texture* _refTexture;
  bool _ownRefractionTexture;

}; // end of class RefractionPostProcess

} // end of namespace BABYLON

#endif // end of BABYLON_POSTPROCESS_REFRACTION_POST_PROCESS_H
