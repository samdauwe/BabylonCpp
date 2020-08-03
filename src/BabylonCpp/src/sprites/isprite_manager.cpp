#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

ISpriteManager::ISpriteManager()
    : scene{this, &ISpriteManager::get_scene}
    , texture{this, &ISpriteManager::get_texture, &ISpriteManager::set_texture}
{
}

ISpriteManager::~ISpriteManager() = default;

} // end of namespace BABYLON
