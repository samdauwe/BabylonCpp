#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/core/random.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/extensions/hexplanetgeneration/icosphere.h>
#include <babylon/extensions/hexplanetgeneration/utils/xor_shift_128.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {

namespace Samples {

class HexIcoSphereScene : public IRenderableScene {

public:
  HexIcoSphereScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
  {
  }

  ~HexIcoSphereScene() override = default;

  const char* getName() override
  {
    return "Hex Icosphere Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position to (x:0, y:5, z:-2.8)
    auto camera = FreeCamera::New("camera1", Vector3(0.f, 0.f, -2.8f), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a Sun & Moon
    auto sun        = HemisphericLight::New("sun", Vector3(0.f, 1.f, 0.f), scene);
    sun->intensity  = 0.96f;
    auto moon       = HemisphericLight::New("moon", Vector3(0.f, -1.f, 0.f), scene);
    moon->intensity = 0.25f;

    // Create the icosphere
    _hexIcosphere = _createHexIcosphere(10, 20.f, scene);

    _scene->registerBeforeRender([this](Scene*, EventState&) {
      _hexIcosphere->rotation().y += -0.0005f * _scene->getAnimationRatio();
      _hexIcosphere->rotation().x += (-0.0005f / 4.f) * _scene->getAnimationRatio();
    });
  }

private:
  MeshPtr _createHexIcosphere(std::size_t degree, float scale, Scene* scene)
  {
    auto material = StandardMaterial::New("mat", scene);
    Extensions::XorShift128 random;
    Extensions::IcosahedronMesh icosahedronMesh
      = Extensions::Icosphere::generateIcosahedronMesh(degree, scale, random);

    Uint32Array indices;
    Float32Array colors;
    Float32Array positions;

    // Generate dual polyhedron position and face indices
    for (auto& node : icosahedronMesh.nodes) {
      auto relativeZeroIndex = static_cast<unsigned int>(positions.size() / 3);

      const auto rand = Math::randomList(0.f, 1.f, 2);
      const Color3 color(0.f, rand[0] * 0.5f, rand[1] * 1.f);

      // Get all the centroids of the faces adjacent to this vertex
      for (auto& fi : node.f) {
        const Vector3& centroid = icosahedronMesh.faces[fi].centroid;
        stl_util::concat(positions, {centroid.x, centroid.y, centroid.z});
        stl_util::concat(colors, {color.r, color.g, color.b, 1.f});
      }

      for (unsigned int i = relativeZeroIndex; i < (positions.size() / 3) - 2; ++i) {
        stl_util::concat(indices, {relativeZeroIndex, i + 1, i + 2});
      }
    }

    auto mesh             = Mesh::New("planet", scene);
    mesh->useVertexColors = true;

    auto vertexData       = std::make_unique<VertexData>();
    vertexData->indices   = std::move(indices);
    vertexData->positions = std::move(positions);
    vertexData->colors    = std::move(colors);

    Float32Array normals;
    VertexData::ComputeNormals(positions, indices, normals);
    vertexData->normals = std::move(normals);
    vertexData->applyToMesh(*mesh, false);

    mesh->material = material;

    return mesh;
  }

private:
  MeshPtr _hexIcosphere;

}; // end of class HexIcosphereScene

BABYLON_REGISTER_SAMPLE("Extensions", HexIcoSphereScene)

} // end of namespace Samples
} // end of namespace BABYLON
