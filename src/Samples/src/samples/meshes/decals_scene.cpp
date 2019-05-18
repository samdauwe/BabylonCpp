#include <babylon/samples/meshes/decals_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/lights/point_light.h>
#include <babylon/loading/scene_loader.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

DecalsScene::DecalsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas)
    , _camera{nullptr}
    , _decalMaterial{nullptr}
    , _onPointerDown{nullptr}
{
}

DecalsScene::~DecalsScene()
{
}

const char* DecalsScene::getName()
{
  return "Decals Scene";
}

void DecalsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // Adding a light
  auto light = PointLight::New("Hemi", Vector3(0.f, 1.f, 0.f), scene);

  // Adding an Arc Rotate Camera
  _camera = ArcRotateCamera::New("Camera", -1.85f, 1.2f, 200.f, Vector3::Zero(),
                                 scene);
  _camera->attachControl(canvas, true);

  // The first parameter can be used to specify which mesh to import. Here we
  // import all meshes
  SceneLoader::ImportMesh(
    {"Shcroendiger'scat"}, "/scenes/", "SSAOcat.babylon", scene,
    [this, &scene,
     &canvas](const std::vector<AbstractMeshPtr>& newMeshes,
              const std::vector<IParticleSystemPtr>& /*newParticleSystems*/,
              const std::vector<SkeletonPtr>& /*newSkeletons*/,
              const std::vector<AnimationGroupPtr>& /*newAnimationGroups*/) {
      // Set the target of the camera to the first imported mesh
      if (!newMeshes.empty()) {
        auto cat = newMeshes[0];

        // Set the target of the camera to the first imported mesh
        _camera->setTarget(cat.get());

        _decalMaterial           = StandardMaterial::New("decalMat", scene);
        auto diffuseTexture      = Texture::New("/textures/impact.png", scene);
        diffuseTexture->hasAlpha = true;
        _decalMaterial->diffuseTexture = diffuseTexture;
        _decalMaterial->zOffset        = -2.f;

        _onPointerDown = [this, &scene, &cat](PointerEvent&& evt) -> void {
          if (evt.button != MouseButtonType::LEFT) {
            return;
          }

          // Check if we are under a mesh
          auto pickInfo
            = scene->pick(scene->pointerX, scene->pointerY,
                          [&cat](const AbstractMeshPtr& mesh) -> bool {
                            return mesh == cat;
                          });
          if (pickInfo.has_value() && pickInfo->hit) {
            Vector3 decalSize(10.f, 10.f, 10.f);

            auto newDecal
              = Mesh::CreateDecal("decal", cat, *pickInfo->pickedPoint,
                                  *pickInfo->getNormal(true), decalSize);
            newDecal->material = _decalMaterial;
          }
        };
      };

      canvas->addMouseEventListener(EventType::MOUSE_BUTTON_DOWN,
                                    _onPointerDown, false);
    });
}

} // end of namespace Samples
} // end of namespace BABYLON
