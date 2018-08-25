#include <babylon/samples/meshes/colored_triangle_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_data.h>

namespace BABYLON {
namespace Samples {

ColoredTriangleScene::ColoredTriangleScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
{
}

ColoredTriangleScene::~ColoredTriangleScene()
{
}

const char* ColoredTriangleScene::getName()
{
  return "Colored Triangle Scene";
}

void ColoredTriangleScene::initializeScene(ICanvas* canvas, Scene* scene)
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

  auto vertexData = ::std::make_unique<VertexData>();

  vertexData->positions = {
    -1, -1, 0, // left bottom
    1,  -1, 0, // top middel
    0,  1,  0  // right bottom
  };

  vertexData->indices = {2, 1, 0};

  vertexData->colors = {
    1, 0, 0, 0, // red   - left
    0, 1, 0, 0, // blue  - top
    0, 0, 1, 0  // green - right
  };

  // https://doc.babylonjs.com/resources/normals
  vertexData->normals = {
    0, 0, 1, //
    0, 0, 1, //
    0, 0, 1  //
  };

  vertexData->applyToMesh(*customMesh);
  customMesh->flipFaces(true);
}

} // end of namespace Samples
} // end of namespace BABYLON
