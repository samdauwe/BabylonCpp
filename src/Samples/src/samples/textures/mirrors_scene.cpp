#include <babylon/samples/textures/mirrors_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/mirror_texture.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>

namespace BABYLON {
namespace Samples {

MirrorsScene::MirrorsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _box1{nullptr}, _step{0.1f}
{
}

MirrorsScene::~MirrorsScene()
{
}

const char* MirrorsScene::getName()
{
  return "Mirrors Scene";
}

void MirrorsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // scene->clearColor = Color3::Black();

  // Setup environment
  auto camera = ArcRotateCamera::New("Camera", -Math::PI_2, 1.57f, 180.f,
                                     Vector3::Zero(), scene);
  camera->attachControl(canvas);

  // Ground
  auto ground                  = Mesh::CreateGround("grd", 150, 150, 1, scene);
  ground->position             = Vector3(0.f, -80.f, 0.f);
  auto grdMaterial             = StandardMaterial::New("gmat", scene);
  grdMaterial->diffuseTexture  = Texture::New("textures/orient.jpg", scene);
  grdMaterial->emissiveTexture = Texture::New("textures/orient.jpg", scene);
  ground->material             = grdMaterial;

  // Box
  _box1                          = Mesh::CreateBox("Box1", 8.0, scene);
  _box1->position                = Vector3(0.f, 0.f, -8.f);
  auto simpleMaterial1           = StandardMaterial::New("smat1", scene);
  simpleMaterial1->emissiveColor = Color3::Red();
  _box1->material                = simpleMaterial1;

  // The planes meshes
  const std::array<Vector3, 5> planePositions{{
    Vector3(0.f, -30.f, 0.f), // Bottom plane
    Vector3(0.f, 0.f, 25.f),  // Back plane
    Vector3(0.f, 30.f, 0.f),  // Top plane
    Vector3(-40.f, 0.f, 0.f), // Left plane
    Vector3(40.f, 0.f, 0.f)   // Right plane
  }};

  const std::array<Vector3, 5> planeRotations{{
    Vector3(Math::PI_2, 0.f, 0.f),  // Bottom plane
    Vector3(0.f, 0.f, 0.f),         // Back plane
    Vector3(-Math::PI_2, 0.f, 0.f), // Top plane
    Vector3(0.f, -Math::PI_4, 0.f), // Left plane
    Vector3(0.f, Math::PI_4, 0.f)   // Right plane
  }};

  std::array<MeshPtr, 5> planes;
  for (unsigned int i = 0; i < 5; ++i) {
    auto iStr                  = std::to_string(i);
    planes[i]                  = Mesh::CreatePlane("plane" + iStr, 70, scene);
    planes[i]->position        = planePositions[i];
    planes[i]->rotation        = planeRotations[i];
    planes[i]->showBoundingBox = true;
  }

  // Vertices data
  const auto getPointsTranformed
    = [](const MeshPtr& mesh,
         const IndicesArray& indexes) -> std::array<Vector3, 3> {
    auto verticesPos = mesh->getVerticesData(VertexBuffer::PositionKind);
    std::array<Vector3, 3> points{
      Vector3::Zero(), //
      Vector3::Zero(), //
      Vector3::Zero()  //
    };
    Vector3::FromArrayToRef(verticesPos, indexes[0] * 3, points[0]);
    Vector3::FromArrayToRef(verticesPos, indexes[1] * 3, points[1]);
    Vector3::FromArrayToRef(verticesPos, indexes[2] * 3, points[2]);
    Vector3::TransformCoordinatesToRef(
      points[0], mesh->computeWorldMatrix(true), points[0]);
    Vector3::TransformCoordinatesToRef(
      points[1], mesh->computeWorldMatrix(true), points[1]);
    Vector3::TransformCoordinatesToRef(
      points[2], mesh->computeWorldMatrix(true), points[2]);
    return points;
  };

  // The plane materials
  for (unsigned int i = 0; i < 5; ++i) {
    auto iStr           = std::to_string(i);
    auto mirrorMaterial = StandardMaterial::New("texture" + iStr, scene);
    auto mirrorTexture
      = MirrorTexture::New("mirror" + iStr, 1024.f, scene, true);
    mirrorTexture->renderList().emplace_back(_box1.get());
    mirrorTexture->level            = 0.6f;
    mirrorMaterial->backFaceCulling = true;
    auto points                     = getPointsTranformed(planes[i], {0, 1, 2});
    mirrorTexture->mirrorPlane
      = Plane::FromPoints(points[0], points[1], points[2]);
    mirrorMaterial->reflectionTexture = mirrorTexture;
    planes[i]->material               = mirrorMaterial;
  }

  // Animations
  scene->registerBeforeRender([this](Scene* /*scene*/, EventState& /*es*/) {
    _box1->rotation().y += _step;
    _box1->rotation().x += _step;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
