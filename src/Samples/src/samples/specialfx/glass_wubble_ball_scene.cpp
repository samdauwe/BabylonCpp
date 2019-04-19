#include <babylon/samples/specialfx/glass_wubble_ball_scene.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/vertex_buffer.h>

namespace BABYLON {
namespace Samples {

GlassWubbleBallScene::GlassWubbleBallScene(ICanvas* iCanvas)
    : IRenderableScene(iCanvas), _t{0.f}, _sphere{nullptr}
{
}

GlassWubbleBallScene::~GlassWubbleBallScene()
{
}

const char* GlassWubbleBallScene::getName()
{
  return "Glass Wubble Ball Scene";
}

void GlassWubbleBallScene::initializeScene(ICanvas* canvas, Scene* scene)
{
  // This creates and positions a free camera (non-mesh)
  auto camera = ArcRotateCamera::New("Camera", 0.f, Math::PI_2, 12.f,
                                     Vector3::Zero(), scene);

  // This targets the camera to scene origin
  camera->setTarget(Vector3::Zero());

  camera->radius = -40.f;

  // This attaches the camera to the canvas
  camera->attachControl(canvas, true);

  // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
  auto light = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
  // Default intensity is 1. Let's dim the light a small amount
  light->intensity = 1.f;

  // Our built-in 'sphere' shape. Params: name, subdivs, size, scene
  _sphere            = Mesh::CreateSphere("sphere1", 32, 25.f, scene);
  auto fsphere       = Mesh::CreateSphere("sphere1", 32, 25.f, scene);
  fsphere->isVisible = false;

  auto mat = StandardMaterial::New("mat", scene);
  // mat.backFaceCulling = false;
  // mat.alpha = 0.7;
  mat->diffuseColor        = Color3(0.4f, 0.5f, 0.7f);
  auto bumpTexture         = Texture::New("/textures/grained_uv.png", scene);
  bumpTexture->uScale      = 4.f;
  bumpTexture->vScale      = 4.f;
  mat->bumpTexture         = bumpTexture;
  auto reflectionTexture   = Texture::New("/textures/spheremap.jpg", scene);
  reflectionTexture->level = 0.6f;
  reflectionTexture->coordinatesMode = TextureConstants::SPHERICAL_MODE;
  mat->reflectionTexture             = reflectionTexture;

  _sphere->material = mat;

  _v  = _sphere->getVerticesData(VertexBuffer::PositionKind);
  _fv = fsphere->getVerticesData(VertexBuffer::PositionKind);

  _scene->registerBeforeRender([this](Scene*, EventState&) {
    for (size_t i = 0; i < _sphere->getTotalVertices(); ++i) {
      auto fx = _fv[i * 3 + 0];
      auto fy = _fv[i * 3 + 1];
      auto fz = _fv[i * 3 + 2];

      _v[i * 3 + 0] = fx + 0.33f * std::sin(_t * 2.15f + fy)
                      + std::cos(_t * 1.45f + fz) + 1.5f;
      _v[i * 3 + 1] = fy + 0.36f * std::cos(_t * 1.15f + fz)
                      + std::sin(_t * 1.45f + fx) + 1.5f;
      _v[i * 3 + 2] = fz + 0.39f * std::sin(_t * 1.15f + fx)
                      + std::cos(_t * 1.45f + fy) + 1.5f;
    }
    _sphere->setVerticesData(VertexBuffer::PositionKind, _v);

    _t += 0.1f;
  });
}

} // end of namespace Samples
} // end of namespace BABYLON
