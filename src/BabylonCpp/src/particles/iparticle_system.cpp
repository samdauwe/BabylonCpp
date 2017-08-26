#include <babylon/particles/iparticle_system.h>

namespace BABYLON {

bool IParticleSystem::hasEmitter()
{
  return emitter.is<AbstractMesh*>() || emitter.is<Vector3>();
}

} // end of namespace BABYLON
