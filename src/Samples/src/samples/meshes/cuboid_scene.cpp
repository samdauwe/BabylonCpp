#include <babylon/samples/meshes/cuboid_scene.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace Samples {

CuboidScene::CuboidScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _cuboid{nullptr}
{
}

CuboidScene::~CuboidScene()
{
}

const char* CuboidScene::getName()
{
  return "Cuboid Scene";
}

void CuboidScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Create a FreeCamera, and set its position to (x:0, y:5, z:-10)
  auto camera = FreeCamera::New("camera1", Vector3(0, 15, -30), scene);

  // Target the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // Attach the camera to the canvas
  camera->attachControl(canvas, true);

  // Create a basic light, aiming 0,1,0 - meaning, to the sky
  auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Create a 3D rectangle or cuboid directly without scaling a cube
  _cuboid = CreateCuboid("rect3", 10, 20, 5, scene);

  // Animation
  scene->registerBeforeRender([this](Scene*, EventState&) {
    _cuboid->rotation().y += 0.008f * getScene()->getAnimationRatio();
  });
}

Mesh* CuboidScene::CreateCuboid(const std::string& name, float length,
                                float width, float height, Scene* scene)
{
  auto cuboid                        = Mesh::New(name, scene);
  std::vector<Vector3> normalsSource = {
    Vector3(0.f, 0.f, 1.f),  // z
    Vector3(0.f, 0.f, -1.f), // z
    Vector3(1.f, 0.f, 0.f),  // x
    Vector3(-1.f, 0.f, 0.f), // x
    Vector3(0.f, 1.f, 0.f),  // y
    Vector3(0.f, -1.f, 0.f)  // y
  };

  Uint32Array indices;
  Float32Array positions;
  Float32Array normals;
  Float32Array uvs;

  // Create each face in turn.
  for (unsigned int index = 0; index < normalsSource.size(); ++index) {
    auto& normal = normalsSource[index];

    // Get two vectors perpendicular to the face normal and to each other.
    Vector3 side1(normal.y, normal.z, normal.x);
    auto side2 = Vector3::Cross(normal, side1);

    // Six indices (two triangles) per face.
    int32_t verticesLength = static_cast<int32_t>(positions.size() / 3);
    indices.emplace_back(verticesLength);
    indices.emplace_back(verticesLength + 1);
    indices.emplace_back(verticesLength + 2);

    indices.emplace_back(verticesLength);
    indices.emplace_back(verticesLength + 2);
    indices.emplace_back(verticesLength + 3);

    // Four vertices per face.
    // - Vertex 1
    auto vertex = normal.subtract(side1).subtract(side2);
    stl_util::concat(positions,
                     {vertex.x * length / 2.f, vertex.y * height / 2.f,
                      vertex.z * width / 2.f});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {1.f, 1.f});
    // - Vertex 2
    vertex = normal.subtract(side1).add(side2);
    stl_util::concat(positions,
                     {vertex.x * length / 2.f, vertex.y * height / 2.f,
                      vertex.z * width / 2.f});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {0.f, 1.f});
    // - Vertex 3
    vertex = normal.add(side1).add(side2);
    stl_util::concat(positions,
                     {vertex.x * length / 2.f, vertex.y * height / 2.f,
                      vertex.z * width / 2.f});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {0.0, 0.0});
    // - Vertex 4
    vertex = normal.add(side1).subtract(side2);
    stl_util::concat(positions,
                     {vertex.x * length / 2.f, vertex.y * height / 2.f,
                      vertex.z * width / 2.f});
    stl_util::concat(normals, {normal.x, normal.y, normal.z});
    stl_util::concat(uvs, {1.0, 0.0});
  }
  cuboid->setVerticesData(VertexBuffer::PositionKind, positions);
  cuboid->setVerticesData(VertexBuffer::NormalKind, normals);
  cuboid->setVerticesData(VertexBuffer::UVKind, uvs);
  cuboid->setIndices(indices);
  return cuboid;
}

} // end of namespace Samples
} // end of namespace BABYLON
