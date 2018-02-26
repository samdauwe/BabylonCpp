#include <babylon/samples/generic/simple_car_following_path_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/axis.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

SimpleCarFollowingPathScene::SimpleCarFollowingPathScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _curveNumberOfPoints{450}
    , _iteration{0}
    , _wheelFI{nullptr}
    , _wheelFO{nullptr}
    , _wheelRI{nullptr}
    , _wheelRO{nullptr}
{
}

SimpleCarFollowingPathScene::~SimpleCarFollowingPathScene()
{
}

const char* SimpleCarFollowingPathScene::getName()
{
  return "Simple Car Following Path Scene";
}

void SimpleCarFollowingPathScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  auto camera
    = ArcRotateCamera::New("camera1", 0, 0, 0, Vector3(0, 0, 0), scene);
  camera->setPosition(Vector3(0, 60, -130));
  camera->attachControl(canvas, true);

  // Create a light
  auto light = HemisphericLight::New("light1", Vector3(1.f, 0.5f, 0.f), scene);
  light->intensity = 0.95f;

  /*----------------------- Car Body -----------------------------------------*/
  // Car Body Material
  auto bodyMaterial          = StandardMaterial::New("body_mat", scene);
  bodyMaterial->diffuseColor = Color3(1.f, 0.25f, 0.25f);
  bodyMaterial->setBackFaceCulling(false);

  // Array of points for trapezium side of car.
  vector_t<Vector3> side{
    Vector3(-4, 2, -2), //
    Vector3(4, 2, -2),  //
    Vector3(5, -2, -2), //
    Vector3(-7, -2, -2) //
  };

  // Close trapezium
  side.emplace_back(side[0]);

  // Array of points for the extrusion path
  const vector_t<Vector3> extrudePath = {Vector3(0, 0, 0), Vector3(0, 0, 4)};

  // Create body and apply material
  ExtrudeShapeOptions extrudeShapeOptions;
  extrudeShapeOptions.shape = side;
  extrudeShapeOptions.path  = extrudePath;
  extrudeShapeOptions.cap   = Mesh::NO_CAP();
  _carBody = MeshBuilder::ExtrudeShape("body", extrudeShapeOptions, scene);
  _carBody->setMaterial(bodyMaterial);
  /*----------------------- End Car Body -------------------------------------*/

  /*----------------------- Wheels--------------------------------------------*/
  // Create the wheel Material
  auto wheelMaterial = StandardMaterial::New("wheel_mat", scene);

  // Lambda that creates a wheel
  const auto createWheel = [this, &scene,
                            &wheelMaterial](const string_t& wheelName,
                                            const Vector3& position) {
    // Create wheel front inside and apply material
    CylinderOptions cylinderOptions(3.f);
    cylinderOptions.height       = 1.f;
    cylinderOptions.tessellation = 24;
    // - Set color for wheel tread as black
    cylinderOptions.faceColors[1] = Color4(0, 0, 0);
    // - Set texture for flat face of wheel
    cylinderOptions.faceUV[0] = Vector4(0, 0, 1, 1);
    cylinderOptions.faceUV[2] = Vector4(0, 0, 1, 1);
    auto wheel = MeshBuilder::CreateCylinder(wheelName, cylinderOptions, scene);
    wheel->setMaterial(wheelMaterial);
    // Rotate wheel so tread in xz plane
    auto axis = Axis::X();
    wheel->rotate(axis, Math::PI_2, Space::WORLD);
    wheel->setParent(_carBody);
    wheel->setPosition(position);
    return wheel;
  };

  // Create the 4 wheels of the car
  _wheelFI = createWheel("wheelFI", Vector3(-4.5f, -2.f, -2.8f));
  _wheelFO = createWheel("wheelFO", Vector3(-4.5f, -2.f, 2.8f));
  _wheelRI = createWheel("wheelRI", Vector3(2.5f, -2.f, -2.8f));
  _wheelRO = createWheel("wheelRO", Vector3(2.5f, -2.f, 2.8f));
  /*----------------------- End Wheels----------------------------------------*/

  /*----------------------- Path ---------------------------------------------*/
  // Create array of points to describe the curve
  float r = 50.f;                 // radius
  auto n  = _curveNumberOfPoints; // Number of points
  for (unsigned int i = 0; i < _curveNumberOfPoints + 1; ++i) {
    _points.emplace_back(
      Vector3((r + (r / 5.f) * ::std::sin(8.f * i * Math::PI / n))
                * ::std::sin(2.f * i * Math::PI / n),
              0.f,
              (r + (r / 10.f) * ::std::sin(6.f * i * Math::PI / n))
                * ::std::cos(2.f * i * Math::PI / n)));
  }

  // Draw the curve
  LinesOptions lineOptions;
  lineOptions.points = _points;
  auto track         = MeshBuilder::CreateLines("track", lineOptions, scene);
  track->color       = Color3::Black();
  /*----------------------- End Path------------------------------------------*/

  /*----------------------- Ground -------------------------------------------*/
  auto rInt = static_cast<unsigned int>(r);
  GroundOptions groundOptions;
  groundOptions.width  = 3 * rInt;
  groundOptions.height = 3 * rInt;
  MeshBuilder::CreateGround("ground", groundOptions, scene);
  /*----------------------- End Ground----------------------------------------*/

  /*----------------------- Position and Rotate Car at Start -----------------*/
  _carBody->position().y = 4.f;
  _carBody->position().z = r;

  Path3D path3d(_points);
  _normals   = path3d.getNormals();
  auto zAxis = Axis::Z();
  auto theta = ::std::acos(Vector3::Dot(zAxis, _normals[0]));
  auto yAxis = Axis::Y();
  _carBody->rotate(yAxis, theta, Space::WORLD);
  _startRotation = *_carBody->rotationQuaternion();
  /*----------------------- End Position and Rotate Car at Start -------------*/

  /*----------------------- Animation Loop -----------------------------------*/
  scene->registerAfterRender([this](Scene* /*scene*/, EventState& /*es*/) {
    auto n                 = _curveNumberOfPoints;
    auto i                 = _iteration;
    _carBody->position().x = _points[i].x;
    _carBody->position().z = _points[i].z;
    _wheelFI->rotate(_normals[i], Math::PI / 32.f, Space::WORLD);
    _wheelFO->rotate(_normals[i], Math::PI / 32.f, Space::WORLD);
    _wheelRI->rotate(_normals[i], Math::PI / 32.f, Space::WORLD);
    _wheelRO->rotate(_normals[i], Math::PI / 32.f, Space::WORLD);

    auto theta = ::std::acos(Vector3::Dot(_normals[i], _normals[i + 1]));
    auto dir   = Vector3::Cross(_normals[i], _normals[i + 1]).y;
    dir        = dir / ::std::abs(dir);
    auto yAxis = Axis::Y();
    _carBody->rotate(yAxis, dir * theta, Space::WORLD);

    i          = (i + 1) % (n - 1); // continuous looping
    _iteration = i;

    if (i == 0) {
      _carBody->setRotationQuaternion(_startRotation);
    }
  });
  /*----------------------- End Animation Loop -------------------------------*/
}

} // end of namespace Samples
} // end of namespace BABYLON
