#include <babylon/cameras/free_camera.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/effect.h>
#include <babylon/materials/effect_shaders_store.h>
#include <babylon/materials/shader_material.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/mesh_builder.h>
#include <babylon/samples/samples_index.h>

namespace BABYLON {
namespace Samples {

class ShaderMaterialCarvedTreesScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Attributes\n"
      "attribute vec3 position;\n"
      "attribute vec2 uv;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "\n"
      "// Varying\n"
      "varying vec2 vUV;\n"
      "\n"
      "void main(void) {\n"
      "    gl_Position = worldViewProjection * vec4(position, 1.0);\n"
      "    vUV = uv;\n"
      "}\n";

  /** Pixel (Fragment) Shader **/
  // Carved Trees ( https://www.shadertoy.com/view/MlBGWG )
  static constexpr const char* customFragmentShader
    = "#ifdef GL_ES\n"
      "precision highp float;\n"
      "#endif\n"
      "\n"
      "// Varying\n"
      "varying vec3 vPosition;\n"
      "varying vec3 vNormal;\n"
      "varying vec2 vUV;\n"
      "\n"
      "// Uniforms\n"
      "uniform mat4 worldViewProjection;\n"
      "uniform float iTime;\n"
      "uniform float iAspectRatio;\n"
      "uniform vec2 iResolution;\n"
      "\n"
      "//(if mousex+y>0...)\n"
      "#define time iTime * .5\n"
      "#define resolution iResolution.xy\n"
      "\n"
      "vec3 ldir;\n"
      "float ot;\n"
      "\n"
      "float tree(vec2 p) {\n"
      "  p = p * .72 + vec2(0., 1.32);\n"
      "  ot = 1000.;\n"
      "  for (int i = 0; i < 28; i++) {\n"
      "    float l = dot(p, p);\n"
      "    ot = min(ot, abs(l - .6));\n"
      "    p.x = abs(p.x);\n"
      "    p = p / l * 2. - vec2(.38, .2);\n"
      "  }\n"
      "  return dot(p, p) * .02;\n"
      "}\n"
      "\n"
      "float light(vec2 p) {\n"
      "  vec2 d = vec2(0., .003);\n"
      "  float d1 = tree(p - d.xy) - tree(p + d.xy);\n"
      "  float d2 = tree(p - d.yx) - tree(p + d.yx);\n"
      "  vec3 n1 = vec3(0., d.y, d1);\n"
      "  vec3 n2 = vec3(d.y, 0., d2);\n"
      "  vec3 n = normalize(cross(n1, n2));\n"
      "  float diff = max(0., dot(ldir, n)) * .6;\n"
      "  vec3 r = reflect(vec3(0., 0., 1.), ldir);\n"
      "  float spec = pow(max(0., dot(r, n)), 25.) * .4;\n"
      "  return (diff + spec + .15) * max(0.4, 1. - tree(p));\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 p = gl_FragCoord.xy / resolution.xy - .5;\n"
      "  vec2 aspect = vec2(resolution.x / resolution.y, 1.);\n"
      "  vec3 iMouse = vec3(0.0, 0.0, 0.0);\n"
      "  p *= aspect;\n"
      "  if (iMouse.z > 0.)\n"
      "    p += 3. * (iMouse.xy / iResolution.xy - .5);\n"
      "  p *= 1. + sin(time) * .2;\n"
      "  float a = 2. + cos(time * .3) * .5;\n"
      "  mat2 rot = mat2(sin(a), cos(a), -cos(a), sin(a));\n"
      "  p *= rot;\n"
      "  p += vec2(sin(time), cos(time)) * .2;\n"
      "  vec3 lightpos = vec3(sin(time * 3.) * .8, cos(time) * .9, -1.);\n"
      "  lightpos.xy *= aspect * .5;\n"
      "  ldir = normalize(vec3(p, -tree(p)) + lightpos);\n"
      "  float l = light(p);\n"
      "  ot = max(1. - ot * .7, 0.);\n"
      "  vec3 col = l * vec3(ot * ot * 1.45, ot * .9, ot * ot * .55);\n"
      "  col +=\n"
      "      pow(max(0., .2 - length(p + lightpos.xy)) / .2, 3.) *\n"
      "      vec3(1.2, 1.1, 1.);\n"
      "  col *= pow(max(0., 1. - length(p + lightpos.xy) * .3), 2.5);\n"
      "  gl_FragColor = vec4(col + .03, 1.0);\n"
      "}\n";

public:
  ShaderMaterialCarvedTreesScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }
  ~ShaderMaterialCarvedTreesScene() override = default;

  const char* getName() override
  {
    return "Shader Material Carved Trees Scene";
  }

  void initializeScene(ICanvas* canvas, Scene* scene) override
  {
    // Create a FreeCamera, and set its position to (x:0, y:0, z:-8)
    auto camera = FreeCamera::New("camera1", Vector3(0.f, 0.f, -8.f), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);

    // Create a built-in "box" shape
    const float ratio = static_cast<float>(getEngine()->getRenderWidth())
                        / static_cast<float>(getEngine()->getRenderHeight());
    BoxOptions options;
    options.size            = 5.f;
    options.sideOrientation = Mesh::DEFAULTSIDE;
    options.updatable       = false;
    options.width           = *options.size * ratio;
    auto skybox             = MeshBuilder::CreateBox("skybox", options, scene);

    // Create shader material
    IShaderMaterialOptions shaderMaterialOptions;
    shaderMaterialOptions.attributes = {"position", "uv"};
    shaderMaterialOptions.uniforms
      = {"iTime", "worldViewProjection", "iAspectRatio", "iResolution"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      const Vector2 resolution{static_cast<float>(_engine->getRenderWidth()),
                               static_cast<float>(_engine->getRenderHeight())};
      const float aspectRatio = resolution.x / resolution.y;
      _shaderMaterial->setFloat("iTime", _time);
      _shaderMaterial->setFloat("iAspectRatio", aspectRatio);
      _shaderMaterial->setVector2("iResolution", resolution);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialCarvedTreesScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialCarvedTreesScene)

} // end of namespace Samples
} // end of namespace BABYLON
