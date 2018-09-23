#include <babylon/samples/meshes/circle_curves_from_beziers.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/math/curve3.h>
#include <babylon/mesh/lines_mesh.h>

namespace BABYLON {
namespace Samples {

CircleCurvesFromBeziersScene::CircleCurvesFromBeziersScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _speed{0.02f}
{
}

CircleCurvesFromBeziersScene::~CircleCurvesFromBeziersScene()
{
}

const char* CircleCurvesFromBeziersScene::getName()
{
  return "Circle Curves From Beziers Scene";
}

void CircleCurvesFromBeziersScene::initializeScene(ICanvas* canvas,
                                                   Scene* scene)
{
  // Create a camera
  auto camera = ArcRotateCamera::New("Camera", 0.f, Math::PI / 2.f, 50.f,
                                     Vector3::Zero(), scene);

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("Light", Vector3(0, 1, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.98f;

  // Draw an arbitrary orbit curve
  const auto createCircleCurve = [scene]() {
    const std::vector<Vector3> p{
      Vector3(10.f, 0.f, 0.f),      //
      Vector3(10.f, 5.519f, 0.f),   //
      Vector3(5.519f, 10.f, 0.f),   //
      Vector3(0.f, 10.f, 0.f),      //
      Vector3(-5.519f, 10.f, 0.f),  //
      Vector3(-10.f, 5.519f, 0.f),  //
      Vector3(-10.f, 0.f, 0.f),     //
      Vector3(-10.f, -5.519f, 0.f), //
      Vector3(-5.519f, -10.f, 0.f), //
      Vector3(0.f, -10.f, 0.f),     //
      Vector3(5.519f, -10.f, 0.f),  //
      Vector3(10.f, -5.519f, 0.f),  //
      Vector3(10.f, 0.f, 0.f)       //
    };
    auto arcA      = Curve3::CreateCubicBezier(p[0], p[1], p[2], p[3], 25);
    auto arcB      = Curve3::CreateCubicBezier(p[3], p[4], p[5], p[6], 25);
    auto arcC      = Curve3::CreateCubicBezier(p[6], p[7], p[8], p[9], 25);
    auto arcD      = Curve3::CreateCubicBezier(p[9], p[10], p[11], p[12], 25);
    auto arcCurve1 = arcA.continueCurve3(arcB);
    auto arcCurve2 = arcC.continueCurve3(arcD);
    auto arcCurve  = arcCurve1.continueCurve3(arcCurve2);
    auto circle    = Mesh::CreateLines("circle", arcCurve.getPoints(), scene);
    circle->color  = Color3(1.f, 1.f, 1.f);

    return circle;
  };

  // Circle curve 1
  _circle1               = createCircleCurve();
  _circle1->rotation().x = Math::PI * 0.37f;

  // Circle curve 2
  _circle2               = createCircleCurve();
  _circle2->rotation().x = Math::PI * 0.37f;
  _circle2->rotation().y += Math::PI;
  _circle2->position().y += 8.f;

  _scene->registerBeforeRender([this](Scene*, EventState&) {
    _circle1->rotation().y += _speed;
    _circle2->rotation().y += _speed;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
