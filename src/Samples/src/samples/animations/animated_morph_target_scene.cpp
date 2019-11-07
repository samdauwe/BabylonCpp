#include <babylon/samples/animations/animated_morph_target_scene.h>
#include "babylon/samples/samples_index.h"

namespace BABYLON {
namespace Samples {

  //struct AnimatedMorphTargetScene_RaiiRegisterer {
  //  AnimatedMorphTargetScene_RaiiRegisterer() {
  //    RegisterSample("Animations", "AnimatedMorphTargetScene", [](ICanvas* iCanvas) {
  //      return std::make_unique<AnimatedMorphTargetScene>(iCanvas);
  //    });
  //  }
  //};

  //AnimatedMorphTargetScene_RaiiRegisterer gAnimatedMorphTargetScene_RaiiRegisterer;

#define REGISTER_SAMPLE(categoryName, sampleClassName)                        \
  struct RaiiRegisterer_##sampleClassName {                                   \
    RaiiRegisterer_##sampleClassName() {                                      \
      RegisterSample(categoryName, #sampleClassName, [](ICanvas* iCanvas) {   \
        return std::make_unique<##sampleClassName>(iCanvas);                  \
      });                                                                     \
    }                                                                         \
  };                                                                          \
                                                                              \
  RaiiRegisterer_##sampleClassName gRaiiRegisterer_##sampleClassName;


  REGISTER_SAMPLE("Animations", AnimatedMorphTargetScene);
} // end of namespace Samples
} // end of namespace BABYLON
