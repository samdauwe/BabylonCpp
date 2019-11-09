#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/axis.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/math/quaternion.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Simple Car Following Path. The car will consist of a trapezoid for the
 * body, four cylinders for wheels. The car will be animated to follow a path an
 * remain parallel to it. The wheels will rotate and be given a texture to show
 * that they are rotating. For simplicity all wheels will remain parallel to the
 * car and their rotation will not be matched to the forward speed of the car.
 * The car will move forward by following a sequence of points on the path. The
 * path will be a curved line with large turning radius and will be shown in the
 * scene.
 *
 * @see https://doc.babylonjs.com/samples/car_path
 */
class SimpleCarFollowingPathScene : public IRenderableScene {

public:
  SimpleCarFollowingPathScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _curveNumberOfPoints{450}
      , _iteration{0}
      , _wheelFI{nullptr}
      , _wheelFO{nullptr}
      , _wheelRI{nullptr}
      , _wheelRO{nullptr}
  {
  }
  ~SimpleCarFollowingPathScene()
  {
  }

  const char* getName() override
  {
    return "Simple Car Following Path Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f,
                                       Vector3(0.f, 0.f, 0.f), scene);
    camera->setPosition(Vector3(0.f, 60.f, -130.f));
    camera->attachControl(canvas, true);
  
    // Create a light
    auto light = HemisphericLight::New("light1", Vector3(1.f, 0.5f, 0.f), scene);
    light->intensity = 0.95f;
  
    /*----------------------- Car Body -----------------------------------------*/
    // Car Body Material
    auto bodyMaterial             = StandardMaterial::New("body_mat", scene);
    bodyMaterial->diffuseColor    = Color3(1.f, 0.25f, 0.25f);
    bodyMaterial->backFaceCulling = false;
  
    // Array of points for trapezium side of car.
    std::vector<Vector3> side{
      Vector3(-4.f, 2.f, -2.f), //
      Vector3(4.f, 2.f, -2.f),  //
      Vector3(5.f, -2.f, -2.f), //
      Vector3(-7.f, -2.f, -2.f) //
    };
  
    // Close trapezium
    side.emplace_back(side[0]);
  
    // Array of points for the extrusion path
    const std::vector<Vector3> extrudePath
      = {Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, 4.f)};
  
    // Create body and apply material
    ExtrudeShapeOptions extrudeShapeOptions;
    extrudeShapeOptions.shape = side;
    extrudeShapeOptions.path  = extrudePath;
    extrudeShapeOptions.cap   = Mesh::NO_CAP;
    _carBody = MeshBuilder::ExtrudeShape("body", extrudeShapeOptions, scene);
    _carBody->material = bodyMaterial;
    /*----------------------- End Car Body -------------------------------------*/
  
    /*----------------------- Wheels--------------------------------------------*/
    // Create the wheel Material
    auto wheelMaterial = StandardMaterial::New("wheel_mat", scene);
  
    // Lambda that creates a wheel
    const auto createWheel = [this, &scene,
                              &wheelMaterial](const std::string& wheelName,
                                              const Vector3& position) {
      // Create wheel front inside and apply material
      CylinderOptions cylinderOptions;
      cylinderOptions.diameter     = 3.f;
      cylinderOptions.height       = 1.f;
      cylinderOptions.tessellation = 24;
      // - Set color for wheel tread as black
      cylinderOptions.faceColors[1] = Color4(0.f, 0.f, 0.f);
      // - Set texture for flat face of wheel
      cylinderOptions.faceUV[0] = Vector4(0.f, 0.f, 1.f, 1.f);
      cylinderOptions.faceUV[2] = Vector4(0.f, 0.f, 1.f, 1.f);
      auto wheel = MeshBuilder::CreateCylinder(wheelName, cylinderOptions, scene);
      wheel->material = wheelMaterial;
      // Rotate wheel so tread in xz plane
      auto axis = Axis::X();
      wheel->rotate(axis, Math::PI_2, Space::WORLD);
      wheel->setParent(_carBody.get());
      wheel->position = position;
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
        Vector3((r + (r / 5.f) * std::sin(8.f * i * Math::PI / n))
                  * std::sin(2.f * i * Math::PI / n),
                0.f,
                (r + (r / 10.f) * std::sin(6.f * i * Math::PI / n))
                  * std::cos(2.f * i * Math::PI / n)));
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
    auto theta = std::acos(Vector3::Dot(zAxis, _normals[0]));
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
  
      auto theta = std::acos(Vector3::Dot(_normals[i], _normals[i + 1]));
      auto dir   = Vector3::Cross(_normals[i], _normals[i + 1]).y;
      dir        = dir / std::abs(dir);
      auto yAxis = Axis::Y();
      _carBody->rotate(yAxis, dir * theta, Space::WORLD);
  
      i          = (i + 1) % (n - 1); // continuous looping
      _iteration = i;
  
      if (i == 0) {
        _carBody->rotationQuaternion = _startRotation;
      }
    });
    /*----------------------- End Animation Loop -------------------------------*/
  }

private:
  unsigned int _curveNumberOfPoints;
  unsigned int _iteration;
  Quaternion _startRotation;
  MeshPtr _carBody;
  MeshPtr _wheelFI, _wheelFO, _wheelRI, _wheelRO;
  std::vector<Vector3> _points, _normals;

}; // end of class SimpleCarFollowingPathScene


BABYLON_REGISTER_SAMPLE("Meshes", SimpleCarFollowingPathScene)
} // end of namespace Samples
} // end of namespace BABYLON