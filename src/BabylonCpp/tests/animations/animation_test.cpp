#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../test_utils.h"

#include <babylon/animations/animation.h>
#include <babylon/animations/ianimation_key.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/mesh.h>

TEST(Animation, OneKey)
{
#if 0
  using namespace BABYLON;
  {
    auto engine = createSubject();
    auto scene  = BABYLON::Scene::New(engine.get());
    auto box    = Mesh::CreateBox("box", 1.f, scene.get());
    scene->createDefaultCamera();
    auto animation = Animation::New("anim", "position.x", 1, Animation::ANIMATIONTYPE_FLOAT());
    std::vector<IAnimationKey> keys{
      IAnimationKey(0.f, AnimationValue(1.f)),
    };
    animation->setKeys(keys);
    box->animations.emplace_back(animation);
    scene->beginAnimation(box, 0.f, 0.f);
    scene->render();
    EXPECT_EQ(box->position().x, 1.f);
  }
#endif
}
