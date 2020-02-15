#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/lights/point_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief Reflecting One Face Sprite Texture Scene. Example demonstrating how to orientate a sprite
 * on a face with faceUV.
 * @see https://www.babylonjs-playground.com/#ICZEXW#4
 * @see https://doc.babylonjs.com/how_to/createbox_per_face_textures_and_colors
 */
class ReflectingOneFaceSpriteTextureScene : public IRenderableScene {

public:
  ReflectingOneFaceSpriteTextureScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _camera{nullptr}, _pl{nullptr}
  {
  }

  ~ReflectingOneFaceSpriteTextureScene() override = default;

  const char* getName() override
  {
    return "Reflecting One Face Sprite Texture Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    scene->clearColor = Color3(0.5f, 0.5f, 0.5f);

    // camera
    _camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 0.f, Vector3::Zero(), scene);
    _camera->setPosition(Vector3(0.f, 0.f, -8.f));
    _camera->attachControl(canvas, true);

    auto light       = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.7f;

    _pl            = PointLight::New("pl", Vector3::Zero(), scene);
    _pl->intensity = 0.5;

    auto mat            = StandardMaterial::New("mat", scene);
    auto texture        = Texture::New("textures/spriteAtlas.png", scene);
    mat->diffuseTexture = texture;

    auto columns = 6.f; // 6 columns
    auto rows    = 4.f; // 4 rows

    std::array<std::optional<Vector4>, 6> faceUV;
    for (unsigned int i = 0; i < 6; i++) {
      faceUV[i] = Vector4(0.f, 0.f, 1.f / columns, 1.f / rows);
    }

    // overwrite face with sprite coordinates
    auto Ubottom_left = 3.f / columns;
    auto Vbottom_left = 0.f;
    auto Utop_right   = (3.f + 1.f) / columns;
    auto Vtop_right   = 1.f / rows;
    faceUV[1]         = Vector4(Ubottom_left, Vbottom_left, Utop_right, Vtop_right);

    BoxOptions options;
    options.faceUV    = faceUV;
    auto box          = MeshBuilder::CreateBox("box", options, scene);
    box->material     = mat;
    box->position().y = 1.75f;

    // Reflect in Vertical Line - Swap U (horizontal) coordinates
    auto faceUV_V_reflect = faceUV; // shallow clone
    faceUV_V_reflect[1]   = Vector4(Utop_right, Vbottom_left, Ubottom_left, Vtop_right);

    BoxOptions options_V_reflect;
    options_V_reflect.faceUV    = faceUV_V_reflect;
    auto box_V_reflect          = MeshBuilder::CreateBox("box", options_V_reflect, scene);
    box_V_reflect->material     = mat;
    box_V_reflect->position().y = 0.25f;

    // Reflect in Horizontal Line - Swap V (vertical) coordinates
    auto faceUV_H_reflect = faceUV; // shallow clone
    faceUV_H_reflect[1]   = Vector4(Ubottom_left, Vtop_right, Utop_right, Vbottom_left);

    BoxOptions options_H_reflect;
    options_H_reflect.faceUV    = faceUV_H_reflect;
    auto box_H_reflect          = MeshBuilder::CreateBox("box", options_H_reflect, scene);
    box_H_reflect->material     = mat;
    box_H_reflect->position().y = -1.25f;

    // Refect in Vertical and Horizontal Lines - Swap U and V coordinates
    auto faceUV_VH_reflect = faceUV; // shallow clone
    faceUV_VH_reflect[1]   = Vector4(Utop_right, Vtop_right, Ubottom_left, Vbottom_left);

    BoxOptions options_VH_reflect;
    options_VH_reflect.faceUV    = faceUV_VH_reflect;
    auto box_VH_reflect          = MeshBuilder::CreateBox("box", options_VH_reflect, scene);
    box_VH_reflect->material     = mat;
    box_VH_reflect->position().x = 1.5f;
    box_VH_reflect->position().y = 0.25f;

    scene->registerBeforeRender(
      [this](Scene* /*scene*/, EventState& /*es*/) { _pl->position = _camera->position; });
  }

private:
  ArcRotateCameraPtr _camera;
  PointLightPtr _pl;

}; // end of class ReflectingOneFaceSpriteTextureScene

BABYLON_REGISTER_SAMPLE("Textures", ReflectingOneFaceSpriteTextureScene)

} // end of namespace Samples
} // end of namespace BABYLON
