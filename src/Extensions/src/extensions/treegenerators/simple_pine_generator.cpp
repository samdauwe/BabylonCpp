#include <babylon/extensions/treegenerators/simple_pine_generator.h>

#include <babylon/math/vector3.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Extensions {

Mesh* SimplePineGenerator::CreateTree(unsigned int canopies,
                                      unsigned int height,
                                      Material* trunkMaterial,
                                      Material* leafMaterial, Scene* scene)
{
  auto curvePoints = [](unsigned int l, unsigned int t) {
    std::vector<Vector3> path;
    auto step = l / t;
    for (unsigned int i = 0.f; i < l; i += step) {
      path.emplace_back(Vector3(0.f, i, 0.f));
      path.emplace_back(Vector3(0.f, i, 0.f));
    }
    return path;
  };

  auto nbL            = canopies + 1;
  auto nbS            = height;
  auto curve          = curvePoints(nbS, nbL);
  auto radiusFunction = [nbL](unsigned int i, unsigned int
                              /*distance*/) {
    float fact = 1.f;
    if (i % 2 == 0) {
      fact = 0.5f;
    }
    return (nbL * 2.f - i - 1.f) * fact;
  };

  auto leaves
    = Mesh::CreateTube("tube", curve, 0, 10, radiusFunction, 1, scene);
  auto trunk
    = Mesh::CreateCylinder("trunk", nbS / nbL, nbL * 1.5f - nbL / 2.f - 1.f,
                           nbL * 1.5f - nbL / 2.f - 1.f, 12, 1, scene);

  leaves->material = leafMaterial;
  trunk->material  = trunkMaterial;
  auto tree        = Mesh::CreateBox("", 1, scene);
  tree->isVisible  = false;
  leaves->setParent(tree);
  trunk->setParent(tree);
  return tree;
}

} // end of namespace Extensions
} // end of namespace BABYLON
