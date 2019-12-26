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

class ShaderMaterialSpeedTracerScene : public IRenderableScene {

public:
  /** Vertex Shader **/
  static constexpr const char* customVertexShader
    = R"ShaderCode(
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
  // Speed tracer ( https://www.shadertoy.com/view/Xlt3Dn )
  static constexpr const char* customFragmentShader
    = R"ShaderCode(
#ifdef GL_ES
precision highp float;
#endif

// Varying
varying vec3 vPosition;
varying vec3 vNormal;
varying vec2 vUV;

// Uniforms
uniform mat4 worldViewProjection;
uniform float iTime;
uniform float iAspectRatio;
uniform vec2 iResolution;

// [SH16B] Speed tracer. Created by Reinder Nijhoff 2016
// @reindernijhoff
//
// https://www.shadertoy.com/view/Xlt3Dn
//
// This shader uses code of the Analytical Motionblur 3D shader by
// Inego and a
// grid to trace a lot of spheres.
//

#define RAYCASTSTEPS 30

#define GRIDSIZE 10.
#define GRIDSIZESMALL 7.
#define MAXHEIGHT 30.
#define SPEED 20.
#define FPS 30.
#define MAXDISTANCE 260.
#define MAXSHADOWDISTANCE 20.

#define time iTime

#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)

//--------------------------------------------------------------------
//  1 out, 2 in...
float hash12(vec2 p) {
  vec3 p3 = fract(vec3(p.xyx) * HASHSCALE1);
  p3 += dot(p3, p3.yzx + 19.19);
  return fract((p3.x + p3.y) * p3.z);
}

//--------------------------------------------------------------------
///  2 out, 2 in...
vec2 hash22(vec2 p) {
  vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
  p3 += dot(p3, p3.yzx + 19.19);
  return fract(vec2((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y));
}

//
// intersection functions
//

bool intersectPlane(const in vec3 ro, const in vec3 rd,
                    const in float height, out float dist) {
  if (rd.y == 0.0) {
    return false;
  }

  float d = -(ro.y - height) / rd.y;
  d = min(100000.0, d);
  if (d > 0.) {
    dist = d;
    return true;
  }
  return false;
}

//
// intersect a MOVING sphere
//
// see: Analytical Motionblur 3D
//      https://www.shadertoy.com/view/MdB3Dw
//
// Created by inigo quilez - iq/2014
//
vec2 iSphere(const in vec3 ro, const in vec3 rd, const in vec4 sp,
             const in vec3 ve, out vec3 nor) {
  float t = -1.0;
  float s = 0.0;
  nor = vec3(0.0);

  vec3 rc = ro - sp.xyz;
  float A = dot(rc, rd);
  float B = dot(rc, rc) - sp.w * sp.w;
  float C = dot(ve, ve);
  float D = dot(rc, ve);
  float E = dot(rd, ve);
  float aab = A * A - B;
  float eec = E * E - C;
  float aed = A * E - D;
  float k = aed * aed - eec * aab;

  if (k > 0.0) {
    k = sqrt(k);
    float hb = (aed - k) / eec;
    float ha = (aed + k) / eec;

    float ta = max(0.0, ha);
    float tb = min(1.0, hb);

    if (ta < tb) {
      ta = 0.5 * (ta + tb);
      t = -(A - E * ta) -
          sqrt((A - E * ta) * (A - E * ta) -
          (B + C * ta * ta - 2.0 * D * ta));
      nor = normalize((ro + rd * t) - (sp.xyz + ta * ve));
      s = 2.0 * (tb - ta);
    }
  }

  return vec2(t, s);
}

//
// Shade
//

vec3 lig = normalize(vec3(-0.6, 0.7, -0.5));

vec3 shade(const in float d, in vec3 col, const in float shadow,
           const in vec3 nor, const in vec3 ref, const in vec3 sky) {
  float amb = max(0., 0.5 + 0.5 * nor.y);
  float dif = max(0., dot(normalize(nor), lig));
  float spe = pow(clamp(dot(normalize(ref), lig), 0.0, 1.0), 16.0);

  dif *= shadow;

  vec3 lin = 1.20 * dif * vec3(1.00, 0.85, 0.55);
  lin += 0.50 * amb * vec3(0.50, 0.70, 1.00);
  col = col * lin;
  col += spe * dif;

  // fog
  col = mix(col, sky, smoothstep(MAXDISTANCE * .8, MAXDISTANCE, d));

  return col;
}

//
// Scene
//

void getSphereOffset(const in vec2 grid, inout vec2 center) {
  center = (hash22(grid) - vec2(0.5)) * (GRIDSIZESMALL);
}

void getMovingSpherePosition(const in vec2 grid,
                             const in vec2 sphereOffset,
                             inout vec4 center, inout vec3 speed) {
  // falling?
  float s = 0.3 + hash12(grid);

  float t = 14. * s + time / s;
  float y = s * MAXHEIGHT * abs(cos(t));

  speed = vec3(0, MAXHEIGHT * abs(sin(t)), 0) * (1. / FPS);

  vec2 offset = grid + sphereOffset;

  center =
      vec4(offset.x + 0.5 * GRIDSIZE, 1. + y,
           offset.y + 0.5 * GRIDSIZE, 1.);
}

void getSpherePosition(const in vec2 grid, const in vec2 sphereOffset,
                       inout vec4 center) {
  vec2 offset = grid + sphereOffset;
  center = vec4(offset.x + 0.5 * GRIDSIZE, 1.,
                offset.y + 0.5 * GRIDSIZE, 1.);
}

vec3 getSphereColor(vec2 grid) {
  float m = hash12(grid.yx) * 12.;
  return vec3(1. - m * 0.08, m * 0.03, m * 0.06);
}

vec3 render(const in vec3 ro, const in vec3 rd,
            const in vec3 cameraSpeed,
            const in mat3 rot) {
  vec3 nor, ref, speed;

  float dist = MAXDISTANCE;

  vec3 sky = clamp(vec3(1, 1.5, 2.5) * (1.0 - 0.8 * rd.y), vec3(0.),
                   vec3(1.));
  vec3 colBackground, sphereSpeed, col = vec3(0.);

  vec4 sphereCenter;
  vec3 pos = floor(ro / GRIDSIZE) * GRIDSIZE;
  vec2 offset;

  if (intersectPlane(ro, rd, 0., dist)) {
    vec3 interSectionPoint = ro + rd * dist;

    // HMMMMM this is totaly fake. Hopefully I have enough time
    // to find the analytic solution to get a motion blurred
    // checkerboard
    speed = rot * (interSectionPoint.xyz - ro) + cameraSpeed;

    vec2 c1 = mod(interSectionPoint.xz * .25, vec2(2.));

    float w =
        (abs(fract(c1.x * abs(rd.x)) - .5) +
         abs(fract(c1.y * abs(rd.y)) - .5));

    colBackground =
        mix(mod(floor(c1.x) + floor(c1.y), 2.) < 1. ?
                vec3(0.4) : vec3(.6),
            vec3(.5), clamp((w + .8) * .007 * length(speed.xz) *
                FPS, 0., 1.));

    // calculate shadow
    float shadow = 0.;

    vec3 shadowStartPos = interSectionPoint - lig;
    vec2 shadowGridPos = floor((ro + rd * dist).xz / GRIDSIZE);

    for (float x = -1.; x <= 1.; x++) {
      for (float y = -1.; y <= 1.; y++) {
        vec2 gridpos = (shadowGridPos + vec2(x, y)) * GRIDSIZE;
        getSphereOffset(gridpos, offset);

        getMovingSpherePosition(gridpos, -offset, sphereCenter,
                                sphereSpeed);

        vec2 res = iSphere(shadowStartPos, lig, sphereCenter,
                           sphereSpeed + cameraSpeed, nor);
        if (res.x > 0.0) {
          shadow =
              clamp(shadow + mix(res.y, 0.,
                    res.x / MAXSHADOWDISTANCE), 0., 1.);
        }

        getSpherePosition(gridpos, offset, sphereCenter);

        res = iSphere(shadowStartPos, lig, sphereCenter,
                      cameraSpeed, nor);
        if (res.x > 0.0) {
          shadow =
              clamp(shadow + mix(res.y, 0.,
                    res.x / MAXSHADOWDISTANCE), 0., 1.);
        }
      }
    }

    ref = reflect(rd, vec3(0., 1., 0.));
    colBackground =
        shade(dist, colBackground, 1. - shadow,
              vec3(0., 1., 0.), ref, sky);
  } else {
    colBackground = sky;
  }

  // trace grid
  vec3 ri = 1.0 / rd;
  vec3 rs = sign(rd) * GRIDSIZE;
  vec3 dis = (pos - ro + 0.5 * GRIDSIZE + rs * 0.5) * ri;
  vec3 mm = vec3(0.0);

  float alpha = 1.;

  for (int i = 0; i < RAYCASTSTEPS; i++) {
    if (alpha < .01)
      break;

    getSphereOffset(pos.xz, offset);

    getMovingSpherePosition(pos.xz, -offset, sphereCenter,
                            sphereSpeed);

    speed = rot * (sphereCenter.xyz - ro) + sphereSpeed + cameraSpeed;
    vec2 res = iSphere(ro, rd, sphereCenter, speed, nor);
    if (res.x > 0.0) {
      ref = reflect(rd, nor);
      vec3 lcol = shade(res.x, getSphereColor(-offset), 1.,
                        nor, ref, sky);
      col += lcol * res.y * alpha;
      alpha *= (1. - res.y);
    }

    getSpherePosition(pos.xz, offset, sphereCenter);

    speed = rot * (sphereCenter.xyz - ro) + cameraSpeed;
    res = iSphere(ro, rd, sphereCenter, speed, nor);
    if (res.x > 0.0) {
      ref = reflect(rd, nor);
      vec3 lcol = shade(res.x, getSphereColor(-offset), 1.,
                        nor, ref, sky);
      col += lcol * res.y * alpha;
      alpha *= (1. - res.y);
    }

    mm = step(dis.xyz, dis.zyx);
    dis += mm * rs * ri;
    pos += mm * rs;
  }

  col += colBackground * alpha;

  return col;
}

void path(in float time, out vec3 ro, out vec3 ta) {
  ro = vec3(16.0 * cos(0.2 + 0.5 * .4 * time * 1.5) * SPEED,
            5.6 + 3. * sin(time),
            16.0 * sin(0.1 + 0.5 * 0.11 * time * 1.5) * SPEED);
  time += 1.6;
  ta = vec3(16.0 * cos(0.2 + 0.5 * .4 * time * 1.5) * SPEED,
            -.1 + 2. * sin(time),
            16.0 * sin(0.1 + 0.5 * 0.11 * time * 1.5) * SPEED);
}

mat3 setCamera(in float time, out vec3 ro) {
  vec3 ta;

  path(time, ro, ta);
  float roll = -0.15 * sin(.732 * time);

  vec3 cw = normalize(ta - ro);
  vec3 cp = vec3(sin(roll), cos(roll), 0.);
  vec3 cu = normalize(cross(cw, cp));
  vec3 cv = normalize(cross(cu, cw));
  return mat3(cu, cv, cw);
}

void main(void) {
  vec2 q = vUV.xy;
  vec2 p = -1.0 + 2.0 * q;
  p.x *= iResolution.x / iResolution.y;

  // camera
  vec3 ro0, ro1, ta;

  mat3 ca0 = setCamera(time - 1. / FPS, ro0);
  vec3 rd0 = ca0 * normalize(vec3(p.xy, 2.0));

  mat3 ca1 = setCamera(time, ro1);
  vec3 rd1 = ca1 * normalize(vec3(p.xy, 2.0));

  mat3 rot = ca1 * mat3(ca0[0].x, ca0[1].x, ca0[2].x,
                        ca0[0].y, ca0[1].y, ca0[2].y,
                        ca0[0].z, ca0[1].z, ca0[2].z);

  rot -= mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);

  // raytrace
  vec3 col = render(ro0, rd0, ro1 - ro0, rot);

  col = pow(col, vec3(0.5));

  gl_FragColor = vec4(col, 1.0);
}
)ShaderCode";

public:
  ShaderMaterialSpeedTracerScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialSpeedTracerScene() override = default;

  const char* getName() override
  {
    return "Shader Material Speed Tracer Scene";
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

}; // end of class ShaderMaterialSpeedTracerScene

BABYLON_REGISTER_SAMPLE("Shader Material", ShaderMaterialSpeedTracerScene)

} // end of namespace Samples
} // end of namespace BABYLON