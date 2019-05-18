#include <babylon/samples/specialfx/sprites_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {
namespace Samples {

SpritesScene::SpritesScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

SpritesScene::~SpritesScene()
{
}

const char* SpritesScene::getName()
{
  return "Sprites Scene";
}

void SpritesScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create camera and light
  auto camera
    = ArcRotateCamera::New("Camera", 1.f, 0.8f, 8.f, Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  auto light       = PointLight::New("Point", Vector3(5.f, 10.f, 5.f), scene);
  light->intensity = 0.98f;

  // Create a sprite manager to optimize GPU ressources
  // Parameters : name, imgUrl, capacity, cellSize, scene
  auto spriteManagerTrees = SpriteManager::New(
    "treesManager", "textures/palm.png", 2000, 1024, scene);

  // We create 2000 trees at random positions
  for (size_t i = 0; i < 2000; ++i) {
    auto tree        = Sprite::New("tree", spriteManagerTrees);
    tree->position.x = Math::random() * 100.f - 50.f;
    tree->position.z = Math::random() * 100.f - 50.f;
    tree->isPickable = true;

    // Some "dead" trees
    if (std::round(Math::random() * 5.f) == 0.f) {
      tree->angle      = Math::PI * 90.f / 180.f;
      tree->position.y = -0.3f;
    }
  }

  // Create a manager for the player's sprite animation
  auto spriteManagerPlayer
    = SpriteManager::New("playerManager", "textures/player.png", 2, 64, scene);

  // First animated player
  auto player = Sprite::New("player", spriteManagerPlayer);
  player->playAnimation(0, 40, true, 100);
  player->position.y = -0.3f;
  player->size       = 0.3f;
  player->isPickable = true;

  // Second standing player
  auto player2 = Sprite::New("player2", spriteManagerPlayer);
  player2->stopAnimation(); // Not animated
  player2->cellIndex  = 2;  // Going to frame number 2
  player2->position.y = -0.3f;
  player2->position.x = 1.f;
  player2->size       = 0.3f;
  player2->invertU    = -1; // Change orientation
  player2->isPickable = true;

  // Picking
  spriteManagerTrees->isPickable  = true;
  spriteManagerPlayer->isPickable = true;

  _pointerInput = [this](PointerInfo* p, EventState& /*es*/) {
    auto pointerX   = p->pointerEvent.clientX;
    auto pointerY   = p->pointerEvent.clientY;
    auto pickResult = getScene()->pickSprite(pointerX, pointerY);
    if (pickResult->hit) {
      pickResult->pickedSprite->angle += 0.5f;
    }
  };

  scene->onPointerObservable.add(
    _pointerInput, static_cast<int>(PointerEventTypes::POINTERDOWN));
}

} // end of namespace Samples
} // end of namespace BABYLON
