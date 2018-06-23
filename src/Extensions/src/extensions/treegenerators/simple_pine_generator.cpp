#include <babylon/extensions/treegenerators/simple_pine_generator.h>

#include <babylon/core/random.h>
#include <babylon/math/vector3.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Extensions {

Mesh* SimplePineGenerator::CreateTree(Scene* scene, Material* trunkMaterial,
                                      Material* leafMaterial,
                                      unsigned int canopies, float baseRadius,
                                      float height, unsigned int tessellation,
                                      float twist)
{
  if (twist < 0.f || twist > 1.f) {
    twist = 0.f;
  }

  const auto curvePoints = [twist](float l, float t) {
    std::vector<Vector3> path;
    auto step = l / t;
    for (float i = 0.f; i < l; i += step) {
      if (i == 0.f) {
        path.emplace_back(Vector3(0.f, i, 0.f));
        path.emplace_back(Vector3(0.f, i, 0.f));
      }
      else {
        path.emplace_back(Vector3(Math::randomNumber(-twist, twist), i,
                                  Math::randomNumber(-twist, twist)));
        path.emplace_back(Vector3(Math::randomNumber(-twist, twist), i,
                                  Math::randomNumber(-twist, twist)));
      }
    }
    return path;
  };

  auto nbL   = canopies + 1;
  auto nbS   = height;
  auto curve = curvePoints(nbS, nbL);

  const auto radiusFunction = [nbL, baseRadius](unsigned int i, float
                                                /*distance*/) {
    float fact = baseRadius;
    if (i % 2 == 0) {
      fact /= 3;
    }
    return (nbL * 2.f - i - 1.f) * fact;
  };

  auto leaves = Mesh::CreateTube("leaves", curve, 0.f, tessellation,
                                 radiusFunction, Mesh::CAP_START(), scene);
  // leaves->convertToFlatShadedMesh();

  auto trunk
    = Mesh::CreateCylinder("trunk", height / nbL, nbL * 1.5f - nbL / 2.f - 1.f,
                           nbL * 1.5f - nbL / 2.f - 1.f, 12, 1, scene);
  // trunk->convertToFlatShadedMesh();

  leaves->material = leafMaterial;
  trunk->material  = trunkMaterial;

  auto tree       = Mesh::CreateBox("", 1.f, scene);
  tree->isVisible = false;

  leaves->setParent(tree);
  trunk->setParent(tree);

  return tree;
}

} // end of namespace Extensions
} // end of namespace BABYLON
