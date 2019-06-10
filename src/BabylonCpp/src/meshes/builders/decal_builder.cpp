#include <babylon/meshes/builders/decal_builder.h>

namespace BABYLON {

MeshPtr DecalBuilder::CreateDecal(const std::string& /*name*/,
                                  const AbstractMesh& /*sourceMesh*/,
                                  DecalOptions& /*options*/, Scene* /*scene*/)
{
  return nullptr;
}

} // end of namespace BABYLON
