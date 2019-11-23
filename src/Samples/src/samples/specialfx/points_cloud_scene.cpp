#include <babylon/babylon_common.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Points Cloud Scene. This example demonstrates how update the vertices data of a mesh to
 * simulate a point cloud.
 * @see https://www.babylonjs-playground.com/#WZNWVG#14
 */
class PointsCloudScene : public IRenderableScene {

public:
  PointsCloudScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas)
      , _pointsCount{10000}
      , _animatePointCloud{false}
      , _customMesh{nullptr}
  {
  }

  ~PointsCloudScene() override = default;

  const char* getName() override
  {
    return "Points Cloud Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a camera
    auto camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
    camera->setPosition(Vector3(0.f, 0.f, -250.f));
    camera->attachControl(canvas, true);

    // Create a custom mesh
    _customMesh = Mesh::New("custom", scene);

    // Set arrays for positions and indices
    _positions.clear();
    _colors.clear();

    for (size_t p = 0; p < _pointsCount; p++) {
      auto x = Math::random() * 100.f;
      auto y = Math::random() * 100.f;
      auto z = Math::random() * 100.f;
      stl_util::concat(_positions, {x, y, z});
      stl_util::concat(_colors, {Math::random(), Math::random(), Math::random(), 1.f});
    }

    // Set the colors for the vertices
    _customMesh->setVerticesData(VertexBuffer::ColorKind, _colors);

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

    if (_animatePointCloud) {
      scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
        for (size_t p = 0; p < _pointsCount; ++p) {
          _positions[3 * p] += Math::random() - 0.5f;
          _positions[3 * p + 1] += Math::random() - 0.5f;
          _positions[3 * p + 2] += Math::random() - 0.5f;
        }
        _customMesh->updateVerticesData(VertexBuffer::PositionKind, _positions);
      });
    }
  }

private:
  size_t _pointsCount;
  bool _animatePointCloud;
  Float32Array _positions;
  Float32Array _colors;
  MeshPtr _customMesh;

}; // end of struct LensFlaresScene

BABYLON_REGISTER_SAMPLE("Special FX", PointsCloudScene)

} // end of namespace Samples
} // end of namespace BABYLON
