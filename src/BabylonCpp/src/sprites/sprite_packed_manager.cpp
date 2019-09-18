#include <babylon/sprites/sprite_packed_manager.h>

namespace BABYLON {

SpritePackedManager::SpritePackedManager(const std::string& iName,
                                         const std::string& imgUrl,
                                         unsigned int capacity, Scene* scene,
                                         const std::string& spriteJSON,
                                         float epsilon,
                                         unsigned int samplingMode)
    // the cellSize parameter is not used when built from JSON which provides
    // individual cell data, defaults to 64 if JSON load fails
    : SpriteManager{iName,   imgUrl,       capacity, 64,        scene,
                    epsilon, samplingMode, true,     spriteJSON}
{
}

SpritePackedManager::~SpritePackedManager()
{
}

} // end of namespace BABYLON
