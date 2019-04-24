#ifndef BABYLON_SAMPLES_ANIMATIONS_CANNON_BALL_SCENE_H
#define BABYLON_SAMPLES_ANIMATIONS_CANNON_BALL_SCENE_H

#include <vector>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class ArcRotateCamera;
class Mesh;
class PointLight;
class Vector3;
using ArcRotateCameraPtr = std::shared_ptr<ArcRotateCamera>;
using MeshPtr            = std::shared_ptr<Mesh>;
using PointLightPtr      = std::shared_ptr<PointLight>;

namespace Samples {

/**
 * @brief Cannon Ball Scene. Scene demonstrating ball physics on a firing
 * cannon.
 * @see https://www.babylonjs-playground.com/#25OQ8V#13
 */
class CannonBallScene : public IRenderableScene {

public:
  CannonBallScene(ICanvas* iCanvas);
  ~CannonBallScene() override;

  const char* getName() override;
  void initializeScene(ICanvas* canvas, Scene* scene) override;

private:
  /**
   * @brief Nothing happens
   */
  void _nothing(float t);

  /**
   * @brief Cannon buggling
   */
  void _buggling(float t);

  /**
   * @brief Cannon firing
   */
  void _firing(float t);

  /**
   * @brief Flying and bouncing ball
   */
  void _flying(float t);

private:
  ArcRotateCameraPtr _camera;
  PointLightPtr _pl;
  std::vector<Vector3> _axis;
  MeshPtr _cannon;
  MeshPtr _ball;

  // Cannon and ball
  float _altitudeZero;
  float _radius;
  float _ballRadius;

  // Animation timer
  float _dt;
  float _t;

  bool _doBugglingAnimation;

  // Sequences
  float _start;
  float _startBuggling; // seconds after initial start
  float _startFiring;   // idem
  float _startBall;     // idem
  float _x0;            // initial ball fire position x
  float _y0;            // initial ball fire position y
  float _vx;            // x speed ball
  float _vy;            // y speed ball
  float _g;             // gravity
  float _loss;          // bounce energy loss
  float _speedFactor;
  float _buggleFactor;

  float _radiusBuggled; // internal reminder, don't modify
  float _timeBall;      // internal fly timer, don't modify
  float _bounce;        // bounce counter

}; // end of class CannonBallScene

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_ANIMATIONS_CANNON_BALL_SCENE_H
