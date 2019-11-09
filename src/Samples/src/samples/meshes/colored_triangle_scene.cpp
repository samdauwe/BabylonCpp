#include <babylon/samples/samples_index.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Colored Triangle Scene.
 * @see https://www.babylonjs-playground.com/#WTKKF6#4
 */
struct ColoredTriangleScene : public IRenderableScene {

  ColoredTriangleScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
  {
  }
  ~ColoredTriangleScene()
  {
  }

  const char* getName() override
  {
    return "Colored Triangle Scene";
  }
  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    const auto black  = Color3::Black();
    scene->clearColor = Color4(black.r, black.g, black.b);
  
    // https://doc.babylonjs.com/babylon101/lights
    auto light
      = HemisphericLight::New("HemiLight", Vector3(-2.f, 0.f, -20.f), scene);
    light->specular = Color3(0.5f, 0.5f, 0.5f);
  
    // https://doc.babylonjs.com/babylon101/cameras
    auto camera = ArcRotateCamera::New("camera1", -Math::PI_2 + 0.5f, Math::PI_2,
                                       Math::PI, Vector3::Zero(), scene);
    camera->upVector = Vector3(0.f, 1.f, 0.f);
    camera->attachControl(canvas, true);
  
    // https://doc.babylonjs.com/how_to/custom
    auto customMesh = Mesh::New("test", scene);
  
    auto vertexData = std::make_unique<VertexData>();
  
    vertexData->positions = {
      -1.f, -1.f, 0.f, // left bottom
      1.f,  -1.f, 0.f, // top middel
      0.f,  1.f,  0.f  // right bottom
    };
  
    vertexData->indices = {2, 1, 0};
  
    vertexData->colors = {
      1.f, 0.f, 0.f, 0.f, // red   - left
      0.f, 1.f, 0.f, 0.f, // blue  - top
      0.f, 0.f, 1.f, 0.f  // green - right
    };
  
    // https://doc.babylonjs.com/resources/normals
    vertexData->normals = {
      0.f, 0.f, 1.f, //
      0.f, 0.f, 1.f, //
      0.f, 0.f, 1.f  //
    };
  
    vertexData->applyToMesh(*customMesh);
    customMesh->flipFaces(true);
  }

}; // end of struct ColoredTriangleScene


BABYLON_REGISTER_SAMPLE("Meshes", ColoredTriangleScene)
} // end of namespace Samples
} // end of namespace BABYLON