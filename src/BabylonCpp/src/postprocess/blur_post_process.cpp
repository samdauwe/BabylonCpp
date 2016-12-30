#include <babylon/postprocess/blur_post_process.h>

#include <babylon/materials/effect.h>

namespace BABYLON {

BlurPostProcess::BlurPostProcess(const std::string& _name,
                                 const Vector2& _direction, float _blurWidth,
                                 float ratio, Camera* camera,
                                 unsigned int samplingMode, Engine* engine,
                                 bool reusable)
    : PostProcess{_name,
                  "blur",
                  {"screenSize", "direction", "blurWidth"},
                  {},
                  ratio,
                  camera,
                  samplingMode,
                  engine,
                  reusable}
    , direction{_direction}
    , blurWidth{_blurWidth}
{
  onApplyObservable.add([&](Effect* effect) {
    effect->setFloat2("screenSize", static_cast<float>(width),
                      static_cast<float>(height));
    effect->setVector2("direction", direction);
    effect->setFloat("blurWidth", blurWidth);
  });
}

BlurPostProcess::~BlurPostProcess()
{
}

} // end of namespace BABYLON
