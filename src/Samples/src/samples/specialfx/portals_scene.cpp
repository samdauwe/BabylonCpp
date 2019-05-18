#include <babylon/samples/specialfx/portals_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/ishader_material_options.h>
#include <babylon/materials/shader_material.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {
namespace Samples {

PortalsScene::PortalsScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _renderTarget{nullptr}, _depthMaterial{nullptr}
{
  // Vertex shader
  Effect::ShadersStore()["depthVertexShader"] = depthVertexShader;

  // Fragment shader
  Effect::ShadersStore()["depthPixelShader"] = depthPixelShader;
}

PortalsScene::~PortalsScene()
{
}

const char* PortalsScene::getName()
{
  return "Portals Scene";
}

void PortalsScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  auto engine = scene->getEngine();
  auto camera
    = ArcRotateCamera::New("Camera", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
  auto material          = StandardMaterial::New("kosh", scene);
  material->diffuseColor = Color3::Purple();
  auto light = PointLight::New("Omni0", Vector3(-17.6f, 18.8f, -49.9f), scene);

  camera->setPosition(Vector3(0.f, 10.f, -40.f));
  camera->minZ = 0.01f;
  camera->maxZ = 120.f;

  camera->attachControl(canvas, true);

  // Depth material
  IShaderMaterialOptions shaderMaterialOptions;
  shaderMaterialOptions.attributes        = {"position"};
  shaderMaterialOptions.uniforms          = {"worldViewProjection"};
  shaderMaterialOptions.needAlphaBlending = true;
  _depthMaterial
    = ShaderMaterial::New("depth", scene, "depth", shaderMaterialOptions);
  _depthMaterial->backFaceCulling = true;

  // Plane
  auto plane          = Mesh::CreatePlane("map", 10, scene);
  plane->position().z = -20.f;
  plane->scaling().x  = 5.f / engine->getAspectRatio(*scene->activeCamera);
  plane->scaling().y  = 5.f / engine->getAspectRatio(*scene->activeCamera);
  plane->visibility   = 1.f;

  // Render target
  _renderTarget = RenderTargetTexture::New("depth", 1024.f, scene);
  scene->customRenderTargets.emplace_back(_renderTarget);

  _renderTarget->onBeforeRender
    = [this](int* /*faceIndex*/, EventState& /*es*/) {
        for (auto& renderItem : _renderTarget->renderList()) {
          renderItem->_savedMaterial = renderItem->material;
          renderItem->material       = _depthMaterial;
        }
      };

  _renderTarget->onAfterRender
    = [this](int* /*faceIndex*/, EventState& /*es*/) {
        // Restoring previous material
        for (auto& renderItem : _renderTarget->renderList()) {
          renderItem->material = renderItem->_savedMaterial;
        }
      };

  // Spheres
  size_t spheresCount = 20;
  float alpha         = 0.f;
  for (size_t index = 0; index < spheresCount; ++index) {
    auto indexStr = std::to_string(index);
    auto sphere   = Mesh::CreateSphere("Sphere" + indexStr, 32, 3, scene);
    sphere->position().x = 10.f * std::cos(alpha);
    sphere->position().z = 10.f * std::sin(alpha);
    sphere->material     = material;

    alpha += Math::PI2 / spheresCount;
    _renderTarget->renderList().emplace_back(sphere.get());
  }

  // Plane material
  auto mat             = StandardMaterial::New("planeMat", scene);
  mat->diffuseColor    = Color3::Black();
  mat->specularColor   = Color3::Black();
  mat->emissiveTexture = _renderTarget;
  mat->opacityTexture  = _renderTarget;

  plane->material = mat;

  auto sphere2          = Mesh::CreateSphere("Sphere2", 8, 5, scene);
  sphere2->position().x = 10.f;
}

} // end of namespace Samples
} // end of namespace BABYLON
