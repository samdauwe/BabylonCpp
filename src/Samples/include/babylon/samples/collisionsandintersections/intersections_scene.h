#ifndef BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_INTERSECTIONS_SCENE_H
#define BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_INTERSECTIONS_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Mesh;
class StandardMaterial;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;
using MeshPtr             = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Intersections Scene. Scene demonstrating how to detect when meshes
 * intersect each other.
 * @see https://www.babylonjs-playground.com/#KQV9SA#0
 */
class IntersectionsScene : public IRenderableScene {

public:
  IntersectionsScene(ICanvas* iCanvas);
  ~IntersectionsScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  Vector3 _pointToIntersect;

  // Materials
  StandardMaterialPtr _balloon1Material;
  StandardMaterialPtr _balloon2Material;
  StandardMaterialPtr _balloon3Material;

  // Meshes
  MeshPtr _plane1;
  MeshPtr _plane2;
  MeshPtr _balloon1;
  MeshPtr _balloon2;
  MeshPtr _balloon3;

  // Animations
  float _alpha;

}; // end of class IntersectionsScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of
       // BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_INTERSECTIONS_SCENE_H
