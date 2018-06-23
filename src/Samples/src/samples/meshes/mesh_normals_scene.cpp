#include <babylon/samples/meshes/mesh_normals_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Samples {

MeshNormalsScene::MeshNormalsScene(ICanvas* iCanvas) : IRenderableScene(iCanvas)
{
}

MeshNormalsScene::~MeshNormalsScene()
{
}

const char* MeshNormalsScene::getName()
{
  return "Mesh Normals Scene";
}

void MeshNormalsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Camera
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, Math::PI_2, 5.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas, true);

  // Light
  auto light = HemisphericLight::New("light1", Vector3(1.f, 0.5f, 0.f), scene);
  light->intensity = 0.8f;

  // Show normals function
  auto showNormals
    = [](Mesh* mesh, float size, const Color3& color, Scene* scene) {
        auto normals   = mesh->getVerticesData(VertexBuffer::NormalKind);
        auto positions = mesh->getVerticesData(VertexBuffer::PositionKind);

        std::vector<std::vector<Vector3>> lines;
        for (unsigned int i = 0; i < normals.size(); i += 3) {
          auto v1 = Vector3::FromArray(positions, i);
          auto v2 = v1.add(Vector3::FromArray(normals, i).scaleInPlace(size));
          std::vector<Vector3> line{v1.add(mesh->position()),
                                    v2.add(mesh->position())};
          lines.emplace_back(line);
        }
        LineSystemOptions options;
        options.lines = std::move(lines);
        auto normalLines
          = MeshBuilder::CreateLineSystem("normalLines", options, scene);
        normalLines->color = color;
        return normalLines;
      };

  // Sphere 1
  SphereOptions options;
  options.segments      = 3;
  auto sphere1          = MeshBuilder::CreateSphere("sphere1", options, scene);
  sphere1->position().y = 1;
  sphere1->material     = StandardMaterial::New("mat1", scene);
  sphere1->material()->setWireframe(true);
  showNormals(sphere1, 0.25f, Color3(1.f, 0.f, 0.f), scene);

  // Sphere 2
  options.segments = 6;
  auto sphere2     = MeshBuilder::CreateSphere("sphere2", options, scene);
  sphere2->convertToFlatShadedMesh();
  sphere2->position().y = -1;
  sphere2->material     = StandardMaterial::New("mat2", scene);
  sphere2->material()->setWireframe(true);
  showNormals(sphere2, 0.25, Color3(1.f, 0.f, 0.f), scene);
}

} // end of namespace Samples
} // end of namespace BABYLON
