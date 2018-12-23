#ifndef BABYLON_SAMPLES_ANIMATIONS_BOUNCING_CUBE_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_BOUNCING_CUBE_SCENE_H

#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class AbstractMesh;
class Mesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MeshPtr         = std::shared_ptr<Mesh>;

namespace Samples {

/**
 * @brief Bouncing Cube Scene.
 *
 * In this simple example, you've got :
 * - a finite world defined by the big box
 * - some random platforms inside it, built with instances
 * - a Mario player, the tiny red box
 *
 * Mario is given an initial random velocity and applied a constant gravity. You
 * can tweak the energy loss against the box walls with the variable restitution
 * (here : 100% = 1.0)
 *
 * When Mario hits a platform coming downward (he's above the platform), he
 * bounces back with some energy loss (variable platformBounceRestitution), when
 * he hits the platform under it, he's just rejected. This is a very simple and
 * naive physics engine because I don't even test if Mario hits the platform
 * borders here. But it is fast. ﻿
 *
 * Note that I also test all the platforms what is not performant at all (well,
 * they aren't very numerous). The use of an octree or any preselection (we know
 * where all the obstacles are in the space before the scene starts !) would be
 * better. ﻿
 *
 * @see http://www.babylonjs-playground.com/#PBVEM#17
 */
class BouncingCubeScene : public IRenderableScene {

public:
  BouncingCubeScene(ICanvas* iCanvas);
  ~BouncingCubeScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  void _applyForces();
  void _checkCollisions();
  void _moveMario();
  void _movePlatforms();

private:
  Vector3 _gravity;
  Vector3 _velocity;
  float _limit;
  float _restitution;
  float _platformBounceRestitution;
  float _k;
  MeshPtr _mario;
  std::vector<AbstractMeshPtr> _platforms;

}; // end of class BouncingCubeScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_BOUNCING_CUBE_SCENE_H
