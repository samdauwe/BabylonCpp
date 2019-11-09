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

class ShaderMaterialSpeedTracerScene : public IRenderableScene {

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
  // Speed tracer ( https://www.shadertoy.com/view/Xlt3Dn )
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
      "// [SH16B] Speed tracer. Created by Reinder Nijhoff 2016\n"
      "// @reindernijhoff\n"
      "//\n"
      "// https://www.shadertoy.com/view/Xlt3Dn\n"
      "//\n"
      "// This shader uses code of the Analytical Motionblur 3D shader by\n"
      "// Inego and a\n"
      "// grid to trace a lot of spheres.\n"
      "//\n"
      "\n"
      "#define RAYCASTSTEPS 30\n"
      "\n"
      "#define GRIDSIZE 10.\n"
      "#define GRIDSIZESMALL 7.\n"
      "#define MAXHEIGHT 30.\n"
      "#define SPEED 20.\n"
      "#define FPS 30.\n"
      "#define MAXDISTANCE 260.\n"
      "#define MAXSHADOWDISTANCE 20.\n"
      "\n"
      "#define time iTime\n"
      "\n"
      "#define HASHSCALE1 .1031\n"
      "#define HASHSCALE3 vec3(.1031, .1030, .0973)\n"
      "#define HASHSCALE4 vec4(1031, .1030, .0973, .1099)\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "//  1 out, 2 in...\n"
      "float hash12(vec2 p) {\n"
      "  vec3 p3 = fract(vec3(p.xyx) * HASHSCALE1);\n"
      "  p3 += dot(p3, p3.yzx + 19.19);\n"
      "  return fract((p3.x + p3.y) * p3.z);\n"
      "}\n"
      "\n"
      "//--------------------------------------------------------------------\n"
      "///  2 out, 2 in...\n"
      "vec2 hash22(vec2 p) {\n"
      "  vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);\n"
      "  p3 += dot(p3, p3.yzx + 19.19);\n"
      "  return fract(vec2((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y));\n"
      "}\n"
      "\n"
      "//\n"
      "// intersection functions\n"
      "//\n"
      "\n"
      "bool intersectPlane(const in vec3 ro, const in vec3 rd,\n"
      "                    const in float height, out float dist) {\n"
      "  if (rd.y == 0.0) {\n"
      "    return false;\n"
      "  }\n"
      "\n"
      "  float d = -(ro.y - height) / rd.y;\n"
      "  d = min(100000.0, d);\n"
      "  if (d > 0.) {\n"
      "    dist = d;\n"
      "    return true;\n"
      "  }\n"
      "  return false;\n"
      "}\n"
      "\n"
      "//\n"
      "// intersect a MOVING sphere\n"
      "//\n"
      "// see: Analytical Motionblur 3D\n"
      "//      https://www.shadertoy.com/view/MdB3Dw\n"
      "//\n"
      "// Created by inigo quilez - iq/2014\n"
      "//\n"
      "vec2 iSphere(const in vec3 ro, const in vec3 rd, const in vec4 sp,\n"
      "             const in vec3 ve, out vec3 nor) {\n"
      "  float t = -1.0;\n"
      "  float s = 0.0;\n"
      "  nor = vec3(0.0);\n"
      "\n"
      "  vec3 rc = ro - sp.xyz;\n"
      "  float A = dot(rc, rd);\n"
      "  float B = dot(rc, rc) - sp.w * sp.w;\n"
      "  float C = dot(ve, ve);\n"
      "  float D = dot(rc, ve);\n"
      "  float E = dot(rd, ve);\n"
      "  float aab = A * A - B;\n"
      "  float eec = E * E - C;\n"
      "  float aed = A * E - D;\n"
      "  float k = aed * aed - eec * aab;\n"
      "\n"
      "  if (k > 0.0) {\n"
      "    k = sqrt(k);\n"
      "    float hb = (aed - k) / eec;\n"
      "    float ha = (aed + k) / eec;\n"
      "\n"
      "    float ta = max(0.0, ha);\n"
      "    float tb = min(1.0, hb);\n"
      "\n"
      "    if (ta < tb) {\n"
      "      ta = 0.5 * (ta + tb);\n"
      "      t = -(A - E * ta) -\n"
      "          sqrt((A - E * ta) * (A - E * ta) -\n"
      "          (B + C * ta * ta - 2.0 * D * ta));\n"
      "      nor = normalize((ro + rd * t) - (sp.xyz + ta * ve));\n"
      "      s = 2.0 * (tb - ta);\n"
      "    }\n"
      "  }\n"
      "\n"
      "  return vec2(t, s);\n"
      "}\n"
      "\n"
      "//\n"
      "// Shade\n"
      "//\n"
      "\n"
      "vec3 lig = normalize(vec3(-0.6, 0.7, -0.5));\n"
      "\n"
      "vec3 shade(const in float d, in vec3 col, const in float shadow,\n"
      "           const in vec3 nor, const in vec3 ref, const in vec3 sky) {\n"
      "  float amb = max(0., 0.5 + 0.5 * nor.y);\n"
      "  float dif = max(0., dot(normalize(nor), lig));\n"
      "  float spe = pow(clamp(dot(normalize(ref), lig), 0.0, 1.0), 16.0);\n"
      "\n"
      "  dif *= shadow;\n"
      "\n"
      "  vec3 lin = 1.20 * dif * vec3(1.00, 0.85, 0.55);\n"
      "  lin += 0.50 * amb * vec3(0.50, 0.70, 1.00);\n"
      "  col = col * lin;\n"
      "  col += spe * dif;\n"
      "\n"
      "  // fog\n"
      "  col = mix(col, sky, smoothstep(MAXDISTANCE * .8, MAXDISTANCE, d));\n"
      "\n"
      "  return col;\n"
      "}\n"
      "\n"
      "//\n"
      "// Scene\n"
      "//\n"
      "\n"
      "void getSphereOffset(const in vec2 grid, inout vec2 center) {\n"
      "  center = (hash22(grid) - vec2(0.5)) * (GRIDSIZESMALL);\n"
      "}\n"
      "\n"
      "void getMovingSpherePosition(const in vec2 grid,\n"
      "                             const in vec2 sphereOffset,\n"
      "                             inout vec4 center, inout vec3 speed) {\n"
      "  // falling?\n"
      "  float s = 0.3 + hash12(grid);\n"
      "\n"
      "  float t = 14. * s + time / s;\n"
      "  float y = s * MAXHEIGHT * abs(cos(t));\n"
      "\n"
      "  speed = vec3(0, MAXHEIGHT * abs(sin(t)), 0) * (1. / FPS);\n"
      "\n"
      "  vec2 offset = grid + sphereOffset;\n"
      "\n"
      "  center =\n"
      "      vec4(offset.x + 0.5 * GRIDSIZE, 1. + y,\n"
      "           offset.y + 0.5 * GRIDSIZE, 1.);\n"
      "}\n"
      "\n"
      "void getSpherePosition(const in vec2 grid, const in vec2 sphereOffset,\n"
      "                       inout vec4 center) {\n"
      "  vec2 offset = grid + sphereOffset;\n"
      "  center = vec4(offset.x + 0.5 * GRIDSIZE, 1.,\n"
      "                offset.y + 0.5 * GRIDSIZE, 1.);\n"
      "}\n"
      "\n"
      "vec3 getSphereColor(vec2 grid) {\n"
      "  float m = hash12(grid.yx) * 12.;\n"
      "  return vec3(1. - m * 0.08, m * 0.03, m * 0.06);\n"
      "}\n"
      "\n"
      "vec3 render(const in vec3 ro, const in vec3 rd,\n"
      "            const in vec3 cameraSpeed,\n"
      "            const in mat3 rot) {\n"
      "  vec3 nor, ref, speed;\n"
      "\n"
      "  float dist = MAXDISTANCE;\n"
      "\n"
      "  vec3 sky = clamp(vec3(1, 1.5, 2.5) * (1.0 - 0.8 * rd.y), vec3(0.),\n"
      "                   vec3(1.));\n"
      "  vec3 colBackground, sphereSpeed, col = vec3(0.);\n"
      "\n"
      "  vec4 sphereCenter;\n"
      "  vec3 pos = floor(ro / GRIDSIZE) * GRIDSIZE;\n"
      "  vec2 offset;\n"
      "\n"
      "  if (intersectPlane(ro, rd, 0., dist)) {\n"
      "    vec3 interSectionPoint = ro + rd * dist;\n"
      "\n"
      "    // HMMMMM this is totaly fake. Hopefully I have enough time\n"
      "    // to find the analytic solution to get a motion blurred\n"
      "    // checkerboard\n"
      "    speed = rot * (interSectionPoint.xyz - ro) + cameraSpeed;\n"
      "\n"
      "    vec2 c1 = mod(interSectionPoint.xz * .25, vec2(2.));\n"
      "\n"
      "    float w =\n"
      "        (abs(fract(c1.x * abs(rd.x)) - .5) +\n"
      "         abs(fract(c1.y * abs(rd.y)) - .5));\n"
      "\n"
      "    colBackground =\n"
      "        mix(mod(floor(c1.x) + floor(c1.y), 2.) < 1. ?\n"
      "                vec3(0.4) : vec3(.6),\n"
      "            vec3(.5), clamp((w + .8) * .007 * length(speed.xz) *\n"
      "                FPS, 0., 1.));\n"
      "\n"
      "    // calculate shadow\n"
      "    float shadow = 0.;\n"
      "\n"
      "    vec3 shadowStartPos = interSectionPoint - lig;\n"
      "    vec2 shadowGridPos = floor((ro + rd * dist).xz / GRIDSIZE);\n"
      "\n"
      "    for (float x = -1.; x <= 1.; x++) {\n"
      "      for (float y = -1.; y <= 1.; y++) {\n"
      "        vec2 gridpos = (shadowGridPos + vec2(x, y)) * GRIDSIZE;\n"
      "        getSphereOffset(gridpos, offset);\n"
      "\n"
      "        getMovingSpherePosition(gridpos, -offset, sphereCenter,\n"
      "                                sphereSpeed);\n"
      "\n"
      "        vec2 res = iSphere(shadowStartPos, lig, sphereCenter,\n"
      "                           sphereSpeed + cameraSpeed, nor);\n"
      "        if (res.x > 0.0) {\n"
      "          shadow =\n"
      "              clamp(shadow + mix(res.y, 0.,\n"
      "                    res.x / MAXSHADOWDISTANCE), 0., 1.);\n"
      "        }\n"
      "\n"
      "        getSpherePosition(gridpos, offset, sphereCenter);\n"
      "\n"
      "        res = iSphere(shadowStartPos, lig, sphereCenter,\n"
      "                      cameraSpeed, nor);\n"
      "        if (res.x > 0.0) {\n"
      "          shadow =\n"
      "              clamp(shadow + mix(res.y, 0.,\n"
      "                    res.x / MAXSHADOWDISTANCE), 0., 1.);\n"
      "        }\n"
      "      }\n"
      "    }\n"
      "\n"
      "    ref = reflect(rd, vec3(0., 1., 0.));\n"
      "    colBackground =\n"
      "        shade(dist, colBackground, 1. - shadow,\n"
      "              vec3(0., 1., 0.), ref, sky);\n"
      "  } else {\n"
      "    colBackground = sky;\n"
      "  }\n"
      "\n"
      "  // trace grid\n"
      "  vec3 ri = 1.0 / rd;\n"
      "  vec3 rs = sign(rd) * GRIDSIZE;\n"
      "  vec3 dis = (pos - ro + 0.5 * GRIDSIZE + rs * 0.5) * ri;\n"
      "  vec3 mm = vec3(0.0);\n"
      "\n"
      "  float alpha = 1.;\n"
      "\n"
      "  for (int i = 0; i < RAYCASTSTEPS; i++) {\n"
      "    if (alpha < .01)\n"
      "      break;\n"
      "\n"
      "    getSphereOffset(pos.xz, offset);\n"
      "\n"
      "    getMovingSpherePosition(pos.xz, -offset, sphereCenter,\n"
      "                            sphereSpeed);\n"
      "\n"
      "    speed = rot * (sphereCenter.xyz - ro) + sphereSpeed + cameraSpeed;\n"
      "    vec2 res = iSphere(ro, rd, sphereCenter, speed, nor);\n"
      "    if (res.x > 0.0) {\n"
      "      ref = reflect(rd, nor);\n"
      "      vec3 lcol = shade(res.x, getSphereColor(-offset), 1.,\n"
      "                        nor, ref, sky);\n"
      "      col += lcol * res.y * alpha;\n"
      "      alpha *= (1. - res.y);\n"
      "    }\n"
      "\n"
      "    getSpherePosition(pos.xz, offset, sphereCenter);\n"
      "\n"
      "    speed = rot * (sphereCenter.xyz - ro) + cameraSpeed;\n"
      "    res = iSphere(ro, rd, sphereCenter, speed, nor);\n"
      "    if (res.x > 0.0) {\n"
      "      ref = reflect(rd, nor);\n"
      "      vec3 lcol = shade(res.x, getSphereColor(-offset), 1.,\n"
      "                        nor, ref, sky);\n"
      "      col += lcol * res.y * alpha;\n"
      "      alpha *= (1. - res.y);\n"
      "    }\n"
      "\n"
      "    mm = step(dis.xyz, dis.zyx);\n"
      "    dis += mm * rs * ri;\n"
      "    pos += mm * rs;\n"
      "  }\n"
      "\n"
      "  col += colBackground * alpha;\n"
      "\n"
      "  return col;\n"
      "}\n"
      "\n"
      "void path(in float time, out vec3 ro, out vec3 ta) {\n"
      "  ro = vec3(16.0 * cos(0.2 + 0.5 * .4 * time * 1.5) * SPEED,\n"
      "            5.6 + 3. * sin(time),\n"
      "            16.0 * sin(0.1 + 0.5 * 0.11 * time * 1.5) * SPEED);\n"
      "  time += 1.6;\n"
      "  ta = vec3(16.0 * cos(0.2 + 0.5 * .4 * time * 1.5) * SPEED,\n"
      "            -.1 + 2. * sin(time),\n"
      "            16.0 * sin(0.1 + 0.5 * 0.11 * time * 1.5) * SPEED);\n"
      "}\n"
      "\n"
      "mat3 setCamera(in float time, out vec3 ro) {\n"
      "  vec3 ta;\n"
      "\n"
      "  path(time, ro, ta);\n"
      "  float roll = -0.15 * sin(.732 * time);\n"
      "\n"
      "  vec3 cw = normalize(ta - ro);\n"
      "  vec3 cp = vec3(sin(roll), cos(roll), 0.);\n"
      "  vec3 cu = normalize(cross(cw, cp));\n"
      "  vec3 cv = normalize(cross(cu, cw));\n"
      "  return mat3(cu, cv, cw);\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 q = vUV.xy;\n"
      "  vec2 p = -1.0 + 2.0 * q;\n"
      "  p.x *= iResolution.x / iResolution.y;\n"
      "\n"
      "  // camera\n"
      "  vec3 ro0, ro1, ta;\n"
      "\n"
      "  mat3 ca0 = setCamera(time - 1. / FPS, ro0);\n"
      "  vec3 rd0 = ca0 * normalize(vec3(p.xy, 2.0));\n"
      "\n"
      "  mat3 ca1 = setCamera(time, ro1);\n"
      "  vec3 rd1 = ca1 * normalize(vec3(p.xy, 2.0));\n"
      "\n"
      "  mat3 rot = ca1 * mat3(ca0[0].x, ca0[1].x, ca0[2].x,\n"
      "                        ca0[0].y, ca0[1].y, ca0[2].y,\n"
      "                        ca0[0].z, ca0[1].z, ca0[2].z);\n"
      "\n"
      "  rot -= mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);\n"
      "\n"
      "  // raytrace\n"
      "  vec3 col = render(ro0, rd0, ro1 - ro0, rot);\n"
      "\n"
      "  col = pow(col, vec3(0.5));\n"
      "\n"
      "  gl_FragColor = vec4(col, 1.0);\n"
      "}\n";

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

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialSpeedTracerScene)

} // end of namespace Samples
} // end of namespace BABYLON
