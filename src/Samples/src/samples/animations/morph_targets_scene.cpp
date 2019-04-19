#include <babylon/samples/animations/morph_targets_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/core/random.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/hdr_cube_texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>
#include <babylon/mesh/vertex_data.h>
#include <babylon/morph/morph_target_manager.h>

namespace BABYLON {
namespace Samples {

MorphTargetsScene::MorphTargetsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _target0{nullptr}
    , _target1{nullptr}
    , _target2{nullptr}
    , _target3{nullptr}
{
}

MorphTargetsScene::~MorphTargetsScene()
{
}

const char* MorphTargetsScene::getName()
{
  return "Morph Targets Scene";
}

void MorphTargetsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // This creates and positions a free camera (non-mesh)
  auto camera = ArcRotateCamera::New("camera1", 1.14f, 1.13f, 10.f,
                                     Vector3::Zero(), scene);

  // This targets the camera to scene origin
  camera->setTarget(Vector3::Zero());

  // This attaches the camera to the canvas
  camera->attachControl(canvas, true);

  // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 0.7f;

  // Our built-in 'sphere' shape. Params: name, subdivs, size, scene
  auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);

  auto hdrTexture = HDRCubeTexture::New("/textures/room.hdr", scene, 512);

  auto exposure                         = 0.6f;
  auto contrast                         = 1.6f;
  auto glass                            = PBRMaterial::New("glass", scene);
  glass->reflectionTexture              = hdrTexture;
  glass->refractionTexture              = hdrTexture;
  glass->linkRefractionWithTransparency = true;
  glass->indexOfRefraction              = 0.52f;
  glass->alpha                          = 0.f;
  glass->cameraExposure                 = exposure;
  glass->cameraContrast                 = contrast;
  glass->microSurface                   = 1.f;
  glass->reflectivityColor              = Color3(0.2f, 0.2f, 0.2f);
  glass->albedoColor                    = Color3(0.85f, 0.85f, 0.85f);
  sphere->material                      = glass;

  auto sphere2 = Mesh::CreateSphere("sphere2", 16, 2.f, scene);
  sphere2->setEnabled(false);
  _addSpike(sphere2);

  auto sphere3 = Mesh::CreateSphere("sphere3", 16, 2.f, scene);
  sphere3->setEnabled(false);
  _addSpike(sphere3);

  auto sphere4 = Mesh::CreateSphere("sphere4", 16, 2.f, scene);
  sphere4->setEnabled(false);
  _addSpike(sphere4);

  auto sphere5 = Mesh::CreateSphere("sphere5", 16, 2.f, scene);
  sphere5->setEnabled(false);
  _addSpike(sphere5);

  auto manager               = MorphTargetManager::New();
  sphere->morphTargetManager = manager;

  _target0 = MorphTarget::FromMesh(sphere2, "sphere2", 0.25f);
  manager->addTarget(_target0);

  _target1 = MorphTarget::FromMesh(sphere3, "sphere3", 0.25f);
  manager->addTarget(_target1);

  _target2 = MorphTarget::FromMesh(sphere4, "sphere4", 0.25f);
  manager->addTarget(_target2);

  _target3 = MorphTarget::FromMesh(sphere5, "sphere5", 0.25f);
  manager->addTarget(_target3);

  // Set influences
  _target0->influence = 0.25f;
  _target1->influence = 0.50f;
  _target2->influence = 0.75f;
  _target3->influence = 1.00f;
}

void MorphTargetsScene::_addSpike(const MeshPtr& mesh)
{
  auto positions = mesh->getVerticesData(VertexBuffer::PositionKind);
  auto normals   = mesh->getVerticesData(VertexBuffer::NormalKind);
  auto indices   = mesh->getIndices();

  for (size_t index = 0; index < 5; ++index) {
    auto randomVertexID
      = static_cast<unsigned int>(mesh->getTotalVertices() * Math::random());
    auto position = Vector3::FromArray(positions, randomVertexID * 3);
    auto normal   = Vector3::FromArray(normals, randomVertexID * 3);

    position.addInPlace(normal);

    position.toArray(positions, randomVertexID * 3);
  }

  VertexData::ComputeNormals(positions, indices, normals);
  mesh->updateVerticesData(VertexBuffer::PositionKind, positions, false, false);
  mesh->updateVerticesData(VertexBuffer::NormalKind, normals, false, false);
}

} // end of namespace Samples
} // end of namespace BABYLON
