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
#include <babylon/samples/babylon_register_sample.h>

namespace BABYLON {
namespace Samples {

/**
 * @brief 2020 - A celebration of new year ( https://www.shadertoy.com/view/WdGSDd )
 * @see https://www.shadertoy.com/view/WdGSDd
 */
class ShaderMaterialCelebrationOfNewYear2020 : public IRenderableScene {

public:
  /** Vertex Shader **/
  // 2020 - A celebration of new year
  // source: https://www.shadertoy.com/view/WdGSDd
  static constexpr const char* customVertexShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Attributes
attribute vec3 position;
attribute vec2 uv;

// Uniforms
uniform mat4 worldViewProjection;

// Varying
varying vec2 vUV;

void main(void) {
    gl_Position = worldViewProjection * vec4(position, 1.0);
    vUV = uv;
}
)ShaderCode";

  /** Pixel (Fragment) Shader **/
  static constexpr const char* customFragmentShader = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Varying
varying vec2 vUV;

// Uniforms
uniform float iTime;
uniform vec2 iResolution;

//
// 2020 - A celebration of new year by Philippe Desgranges
// Email: Philippe.desgranges@gmail.com
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
//

// Note: This shader is a fairly standard 2D composition with two layers. The digits
// are produced with bespoke signed distance functions (the fact that 2020 has only two diferent
// digits made the process easier).
// The background is itslef a composition of 3 layers of cellular bokeh with some color tweaks
// similar to techniques shown in BigWings tutorials.
//
// There is no huge technical feat but I wanted to create a warm and colorfull image.
// Tell me what you think :D

#define S(a,b,c) smoothstep(a,b,c)

// outputs a colored shape with a white border from distance field (RGBA premultiplied)
vec4 border(vec3 color, float dist)
{
    vec4 res;

    float aa = 30. / iResolution.x;

    res.a = S(0.25 + aa, 0.25, dist);
    res.rgb = mix(color, vec3(res.a),  S(0.1, 0.1 + aa, dist));

    return res;
}

// Blend a premultiplied rbga color onto rgb
vec3 premulBlend(vec4 src, vec3 dst)
{
    return dst * (1.0 - src.a) + src.rgb;
}

// Blend a premultiplied rbga color onto rgba (accurate alpha handling)
vec4 premulBlend(vec4 src, vec4 dst)
{
    vec4 res;
    res.rgb = dst.rgb * (1.0 - src.a) + src.rgb;
    res.a = 1.0 - (1.0 - src.a) * (1.0 - dst.a);

    return res;
}


// Distance field to the digit 0
float zeroDst(vec2 uv)
{
    float dist;

    uv.y -= 0.5;

    if (uv.y > 0.0) // upper part
    {
      uv.y = pow(uv.y, 1.8);
      dist = length(uv);
    }
    else if (uv.y > -1.1) // middle part
    {
      dist = abs(uv.x);
    }
    else  // lower part
    {
      uv.y += 1.1;
      uv.y = pow(-uv.y, 1.8);
      dist = length(uv);
    }

    return (abs(dist - 0.725) - 0.275);
}

// a box distance function
float box(vec2 p, vec2 b )
{
    vec2 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}

// Distance field to the digit 2
float twoDst(vec2 uv)
{
    uv.y -= 0.5;

    float topBar = box((uv + vec2(0.725, 0.0)) * vec2(1.0, 1.4), vec2(0.275, 0.0));

    if (uv.y > 0.0) // Top 'curve'
    {
      uv.y = pow(uv.y, 1.8);
      float dist = length(uv);
      return max(-topBar, (abs(dist - 0.725) - 0.275));
    }
    else
    {
      float bottomBar = box((uv + vec2(0.0, 1.83)) * vec2(1.0, 1.4), vec2(0.95, 0.299));

      float two = min(topBar, bottomBar);

      if (uv.y > -1.8)
      {
        float curve = (cos(uv.y * 2.0) - 1.0) * 0.7;
        float x = 0.0 + uv.x - curve;
        float mid = abs(uv.y + 0.4) * 0.7;
        float x2 = 0.0 + uv.x - curve - mid * mid * 0.15 + 0.01;

        two = min(two, max(-x + 0.45, x2 -1.0));
      }
      return two;
    }
}


// Coordinate transform from global uv space to charcter space with poition and rotation
vec2 letterUVs(vec2 uv, vec2 pos, float angle)
{
    float c = sin(angle);
    float s = cos(angle);
    float sc = 1.35;
    uv -= pos;
    return uv.x * vec2(s * sc, c) + uv.y * vec2(-c * sc, s);
}

float shadowsIntensity = 0.74;
float shadowRadius = 1.1;

// Character two with outline and shadow (premultiplied RGBA)
vec4 twoCol(vec2 uv, vec3 col, vec2 pos, float angle)
{
    uv = letterUVs(uv, pos, angle);
    vec4 res = border(col, twoDst(uv));

    uv.y += 0.14;
    res.a = min(res.a +  S(shadowRadius, -1.0, twoDst(uv)) * shadowsIntensity, 1.0);

    return res;
}

// Character zero with outline and shadow (premultiplied RGBA)
vec4 zeroCol(vec2 uv, vec3 col, vec2 pos, float angle)
{
    uv = letterUVs(uv, pos, angle);
    vec4 res = border(col, zeroDst(uv));

    uv.y += 0.14;
    res.a = min(res.a +  S(shadowRadius, -1.0, zeroDst(uv)) * shadowsIntensity, 1.0);

    return res;
}


vec3 red = vec3(0.9, 0.01, 0.16);
vec3 yellow = vec3(0.96, 0.70, 0.19); // 248, 181, 51
vec3 green = vec3(0.00, 0.63, 0.34);  //1, 162, 88
vec3 blue = vec3(0.01, 0.57, 0.76);   //5, 142, 197

// 2020 with colors and shadows (premultiplied rgba)
vec4 yearCol(vec2 uv)
{
    float angle = sin(iTime) * 0.3;

    vec4 date = twoCol(uv, red, vec2(-2.5, 0.0), angle);
    date = premulBlend(zeroCol(uv, yellow, vec2(-0.8, 0.0), angle), date);
    date = premulBlend(twoCol(uv, green, vec2(0.8, 0.0), angle), date);
    date = premulBlend(zeroCol(uv, blue, vec2(2.5, 0.0), angle), date);

    return  date;
}

// Borrowed from BigWIngs (random 1 -> 4)
vec4 N14(float t) {
    return fract(sin(t*vec4(123., 104., 145., 24.))*vec4(657., 345., 879., 154.));
}


// Compute a randomized Bokeh spot inside a grid cell
float bokehSpot(vec2 uv, vec2 id, float decimation)
{
    float accum = 0.0;

    for (float x = -1.0; x <= 1.0; x += 1.0)
    {
      for (float y = -1.0; y <= 1.0; y += 1.0)
      {
        vec2 offset = vec2(x, y);
        vec2 cellId = id + offset;
        vec4 rnd = N14(mod(cellId.x, 300.0) * 25.3 + mod(cellId.y, 300.0) * 6.67);

        vec2 cellUV = uv - offset + rnd.yz * 0.5;

        float dst = length(cellUV);

        float radSeed = sin(iTime * 0.02 + rnd.x * 40.0);
        float rad =  (abs(radSeed) - decimation) / (1.0 - decimation);

        float intensity = S(rad, rad - 0.15, dst);

        accum += intensity;
      }
    }

    return accum;
}

// Computes a random layer of bokeh spots
float bokehLayer(vec2 uv, float decimation)
{
    vec2 id = floor(uv);
    vec2 cellUV = (uv - id) - vec2(0.5, 0.5) ;

    float intensity = bokehSpot(cellUV, id, decimation);

    return intensity;
}


// Computes the bokeh background
vec3 bokeh(vec2 uv)
{
    //accumulates several layers of bokeh
    float intensity = bokehLayer(uv * 1.4 + vec2(iTime * 0.3, 0.0), 0.9) * 0.2;
    //intensity += bokehLayer(uv * 0.8 + vec2(200.0 + iTime * 0.3, 134.0), 0.9) * 0.3;
    intensity += bokehLayer(uv * 0.5 + vec2(0.0 + iTime * 0.3, 334.0), 0.95) * 0.15;
    intensity += bokehLayer(uv * 3.0 + vec2(iTime * 0.3, 99.0), 0.95) * 0.05;

    float cDist = max(0.0, 1.0 - length(uv) * 0.05);

    intensity = cDist + intensity;

    // Vary color with intensity
    vec3 chroma = mix(vec3(1.0, 1.0, 0.4), vec3(1.0, 0.7, 0.5), uv.y * 0.1 + 0.4 + intensity * 0.4);

    return chroma * intensity;
}

// Final composition
void main(void)
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = vUV.xy;
    uv -= .5;
    uv *= 15.0;

    vec4 dateCol =  yearCol(uv); // 2020

    vec3 bg = bokeh(uv);

    //add a bit of light
    dateCol.rgb -= uv.y * 0.15 * dateCol.a;
    bg.rgb -= uv.y * 0.03;

    // blend 2020 and BG
    vec3 col = premulBlend(dateCol, bg);

    // Gamma correction to make the image warmer
    float gamma = 0.8;
    col.rgb = pow(col.rgb, vec3(gamma));

    gl_FragColor  = vec4(col,1.0);
}
)ShaderCode";

public:
  ShaderMaterialCelebrationOfNewYear2020(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialCelebrationOfNewYear2020() override = default;

  const char* getName() override
  {
    return "2020 - A celebration of new year";
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
    shaderMaterialOptions.uniforms   = {"iTime", "worldViewProjection", "iResolution"};
    _shaderMaterial = ShaderMaterial::New("boxShader", scene, "custom", shaderMaterialOptions);

    // box + sky = skybox !
    skybox->material = _shaderMaterial;

    // Animation
    scene->onAfterCameraRenderObservable.add([this](Camera*, EventState&) {
      const Vector2 resolution{static_cast<float>(_engine->getRenderWidth()),
                               static_cast<float>(_engine->getRenderHeight())};
      _shaderMaterial->setFloat("iTime", _time);
      _shaderMaterial->setVector2("iResolution", resolution);
      _time += 0.01f * getScene()->getAnimationRatio();
    });
  }

private:
  float _time;
  ShaderMaterialPtr _shaderMaterial;

}; // end of class ShaderMaterialCelebrationOfNewYear2020

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialCelebrationOfNewYear2020)

} // end of namespace Samples
} // end of namespace BABYLON
