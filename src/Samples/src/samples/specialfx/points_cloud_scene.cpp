#include <babylon/samples/specialfx/points_cloud_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {
namespace Samples {

PointsCloudScene::PointsCloudScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _pointsCount{10000}, _customMesh{nullptr}
{
}

PointsCloudScene::~PointsCloudScene()
{
}

const char* PointsCloudScene::getName()
{
  return "Points Cloud Scene";
}

void PointsCloudScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a camera
  auto camera
    = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
  camera->setPosition(Vector3(0.f, 50.f, -300.f));
  camera->attachControl(canvas, true);

  // Create a custom mesh
  _customMesh = Mesh::New("custom", scene);

  // Set arrays for positions and indices
  Float32Array colors;

  for (size_t p = 0; p < _pointsCount; p++) {
    auto x = Math::random() * 100.f;
    auto y = Math::random() * 100.f;
    auto z = Math::random() * 100.f;
    stl_util::concat(_positions, {x, y, z});
    stl_util::concat(colors,
                     {Math::random(), Math::random(), Math::random(), 1.f});
  }

  // Set the colors for the vertices
  _customMesh->setVerticesData(VertexBuffer::ColorKind, colors);

  VertexData vertexData;

  // Assign positions
  vertexData.positions = _positions;

  // Apply vertexData to custom mesh
  vertexData.applyToMesh(*_customMesh, true);

  auto mat             = StandardMaterial::New("mat", scene);
  mat->emissiveColor   = Color3(1.f, 1.f, 1.f);
  mat->disableLighting = true;
  mat->pointsCloud     = true;
  mat->pointSize       = 3;

  _customMesh->material = mat;

  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    for (size_t p = 0; p < _pointsCount; ++p) {
      _positions[3 * p] += Math::random() - 0.5f;
      _positions[3 * p + 1] += Math::random() - 0.5f;
      _positions[3 * p + 2] += Math::random() - 0.5f;
    }
    _customMesh->updateVerticesData(VertexBuffer::PositionKind, _positions);
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
