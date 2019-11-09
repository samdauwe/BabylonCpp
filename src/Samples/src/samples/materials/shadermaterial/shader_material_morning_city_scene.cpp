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

class ShaderMaterialMorningCityScene : public IRenderableScene {

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
  // Morning city ( https://www.shadertoy.com/view/XsBSRG )
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
      "#define CARS\n"
      "#define I_MAX 70\n"
      "\n"
      "float rand(vec2 n) {\n"
      "  return fract(sin((n.x * 1e2 + n.y * 1e4 + 1475.4526) * 1e-4) * 1e6);\n"
      "}\n"
      "\n"
      "float noise(vec2 p) {\n"
      "  p = floor(p * 200.0);\n"
      "  return rand(p);\n"
      "}\n"
      "vec3 polygonXY(float z, vec2 vert1, vec2 vert2,\n"
      "               vec3 camPos, vec3 rayDir) {\n"
      "  float t = -(camPos.z - z) / rayDir.z;\n"
      "  vec2 cross = camPos.xy + rayDir.xy * t;\n"
      "  if (cross.x > min(vert1.x, vert2.x) &&\n"
      "      cross.x < max(vert1.x, vert2.x) &&\n"
      "      cross.y > min(vert1.y, vert2.y) &&\n"
      "      cross.y < max(vert1.y, vert2.y) &&\n"
      "      dot(rayDir, vec3(cross, z) - camPos) > 0.0) {\n"
      "    float dist = length(camPos - vec3(cross, z));\n"
      "    return vec3(dist, cross.x - min(vert1.x, vert2.x),\n"
      "                cross.y - min(vert1.y, vert2.y));\n"
      "  }\n"
      "\n"
      "  return vec3(101.0, 0.0, 0.0);\n"
      "}\n"
      "vec3 polygonYZ(float x, vec2 vert1, vec2 vert2,\n"
      "               vec3 camPos, vec3 rayDir) {\n"
      "  float t = -(camPos.x - x) / rayDir.x;\n"
      "  vec2 cross1 = camPos.yz + rayDir.yz * t;\n"
      "  if (cross1.x > min(vert1.x, vert2.x) &&\n"
      "      cross1.x < max(vert1.x, vert2.x) &&\n"
      "      cross1.y > min(vert1.y, vert2.y) &&\n"
      "      cross1.y < max(vert1.y, vert2.y) &&\n"
      "      dot(rayDir, vec3(x, cross1) - camPos) > 0.0) {\n"
      "    float dist = length(camPos - vec3(x, cross1));\n"
      "    return vec3(dist, cross1.x - min(vert1.x, vert2.x),\n"
      "                cross1.y - min(vert1.y, vert2.y));\n"
      "  }\n"
      "\n"
      "  return vec3(101.0, 0.0, 0.0);\n"
      "}\n"
      "vec3 polygonXZ(float y, vec2 vert1, vec2 vert2,\n"
      "               vec3 camPos, vec3 rayDir) {\n"
      "  float t = -(camPos.y - y) / rayDir.y;\n"
      "  vec2 cross1 = camPos.xz + rayDir.xz * t;\n"
      "  if (cross1.x > min(vert1.x, vert2.x) &&\n"
      "      cross1.x < max(vert1.x, vert2.x) &&\n"
      "      cross1.y > min(vert1.y, vert2.y) &&\n"
      "      cross1.y < max(vert1.y, vert2.y) &&\n"
      "      dot(rayDir, vec3(cross1.x, y, cross1.y) - camPos) > 0.0) {\n"
      "    float dist = length(camPos - vec3(cross1.x, y, cross1.y));\n"
      "    return vec3(dist, cross1.x - min(vert1.x, vert2.x),\n"
      "                cross1.y - min(vert1.y, vert2.y));\n"
      "  }\n"
      "\n"
      "  return vec3(101.0, 0.0, 0.0);\n"
      "}\n"
      "\n"
      "vec3 textureWall(vec2 pos, vec2 maxPos, vec2 squarer,\n"
      "                 float s, float height,\n"
      "                 float dist, vec3 rayDir, vec3 norm) {\n"
      "  float randB = rand(squarer * 2.0);\n"
      "  vec3 windowColor =\n"
      "      (-0.4 + randB * 0.8) * vec3(0.3, 0.3, 0.0) +\n"
      "      (-0.4 + fract(randB * 10.0) * 0.8) * vec3(0.0, 0.0, 0.3) +\n"
      "      (-0.4 + fract(randB * 10000.0) * 0.8) * vec3(0.3, 0.0, 0.0);\n"
      "  float floorFactor = 1.0;\n"
      "  vec2 windowSize = vec2(0.65, 0.35);\n"
      "  vec3 wallColor =\n"
      "      s * (0.3 + 1.4 * fract(randB * 100.0)) * vec3(0.1, 0.1, 0.1) +\n"
      "      (-0.7 + 1.4 * fract(randB * 1000.0)) * vec3(0.02, 0., 0.);\n"
      "  wallColor *= 1.3;\n"
      "\n"
      "  vec3 color = vec3(0.0);\n"
      "  vec3 conturColor = wallColor / 1.5;\n"
      "  if (height < 0.51) {\n"
      "    windowColor += vec3(0.3, 0.3, 0.0);\n"
      "    windowSize = vec2(0.4, 0.4);\n"
      "    floorFactor = 0.0;\n"
      "  }\n"
      "  if (height < 0.6) {\n"
      "    floorFactor = 0.0;\n"
      "  }\n"
      "  if (height > 0.75) {\n"
      "    windowColor += vec3(0.0, 0.0, 0.3);\n"
      "  }\n"
      "  windowColor *= 1.5;\n"
      "  float wsize = 0.02;\n"
      "  wsize += -0.007 + 0.014 * fract(randB * 75389.9365);\n"
      "  windowSize +=\n"
      "      vec2(0.34 * fract(randB * 45696.9365),\n"
      "           0.50 * fract(randB * 853993.5783));\n"
      "\n"
      "  vec2 contur = vec2(0.0) + (fract(maxPos / 2.0 / wsize)) * wsize;\n"
      "  if (contur.x < wsize) {\n"
      "    contur.x += wsize;\n"
      "  }\n"
      "  if (contur.y < wsize) {\n"
      "    contur.y += wsize;\n"
      "  }\n"
      "\n"
      "  vec2 winPos =\n"
      "      (pos - contur) / wsize / 2.0 - floor(\n"
      "      (pos - contur) / wsize / 2.0);\n"
      "\n"
      "  float numWin = floor((maxPos - contur) / wsize / 2.0).x;\n"
      "\n"
      "  if ((maxPos.x > 0.5 && maxPos.x < 0.6) &&\n"
      "      (((pos - contur).x > wsize * 2.0 * floor(numWin / 2.0)) &&\n"
      "       ((pos - contur).x < wsize * 2.0 + wsize *\n"
      "         2.0 * floor(numWin / 2.0)))) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "\n"
      "  if ((maxPos.x > 0.6 && maxPos.x < 0.7) &&\n"
      "      ((((pos - contur).x > wsize * 2.0 * floor(numWin / 3.0)) &&\n"
      "        ((pos - contur).x < wsize * 2.0 + wsize * 2.0 *\n"
      "         floor(numWin / 3.0))) ||\n"
      "       (((pos - contur).x > wsize * 2.0 * floor(numWin * 2.0 / 3.0))\n"
      "          &&\n"
      "        ((pos - contur).x <\n"
      "         wsize * 2.0 + wsize * 2.0 * floor(numWin * 2.0 / 3.0))))) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "\n"
      "  if ((maxPos.x > 0.7) &&\n"
      "      ((((pos - contur).x > wsize * 2.0 * floor(numWin / 4.0)) &&\n"
      "        ((pos - contur).x < wsize * 2.0 + wsize * 2.0 *\n"
      "          floor(numWin / 4.0))) ||\n"
      "       (((pos - contur).x > wsize * 2.0 * floor(numWin * 2.0 / 4.0))\n"
      "          &&\n"
      "        ((pos - contur).x <\n"
      "         wsize * 2.0 + wsize * 2.0 * floor(numWin * 2.0 / 4.0))) ||\n"
      "       (((pos - contur).x > wsize * 2.0 * floor(numWin * 3.0 / 4.0))\n"
      "          &&\n"
      "        ((pos - contur).x <\n"
      "         wsize * 2.0 + wsize * 2.0 * floor(numWin * 3.0 / 4.0))))) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "  if ((maxPos.x - pos.x < contur.x) ||\n"
      "      (maxPos.y - pos.y < contur.y + 2.0 * wsize) ||\n"
      "      (pos.x < contur.x) ||\n"
      "      (pos.y < contur.y)) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "  if (maxPos.x < 0.14) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * wallColor;\n"
      "  }\n"
      "  vec2 window = floor((pos - contur) / wsize / 2.0);\n"
      "  float random = rand(squarer * s * maxPos.y + window);\n"
      "  float randomZ =\n"
      "      rand(squarer * s * maxPos.y +\n"
      "           floor(vec2((pos - contur).y,\n"
      "                 (pos - contur).y) / wsize / 2.0));\n"
      "  float windows =\n"
      "      floorFactor * sin(randomZ * 5342.475379 +\n"
      "                        (fract(975.568 * randomZ) * 0.15 + 0.05) *\n"
      "                         window.x);\n"
      "\n"
      "  float blH = 0.06 * dist * 600.0 / iResolution.x /\n"
      "              abs(dot(normalize(rayDir.xy), normalize(norm.xy)));\n"
      "  float blV = 0.06 * dist * 600.0 / iResolution.x /\n"
      "              sqrt(abs(1.0 - pow(abs(rayDir.z), 2.0)));\n"
      "\n"
      "  windowColor += vec3(1.0, 1.0, 1.0);\n"
      "  windowColor *= smoothstep(0.5 - windowSize.x / 2.0 - blH,\n"
      "                            0.5 - windowSize.x / 2.0 + blH, winPos.x);\n"
      "  windowColor *= smoothstep(0.5 + windowSize.x / 2.0 + blH,\n"
      "                            0.5 + windowSize.x / 2.0 - blH, winPos.x);\n"
      "  windowColor *= smoothstep(0.5 - windowSize.y / 2.0 - blV,\n"
      "                            0.5 - windowSize.y / 2.0 + blV, winPos.y);\n"
      "  windowColor *= smoothstep(0.5 + windowSize.y / 2.0 + blV,\n"
      "                            0.5 + windowSize.y / 2.0 - blV, winPos.y);\n"
      "\n"
      "  if ((random < 0.05 * (3.5 - 2.5 * floorFactor)) ||\n"
      "      (windows > 0.65)) {\n"
      "    if (winPos.y < 0.5) {\n"
      "      windowColor *= (1.0 - 0.4 * fract(random * 100.0));\n"
      "    }\n"
      "    if ((winPos.y > 0.5) && (winPos.x < 0.5)) {\n"
      "      windowColor *= (1.0 - 0.4 * fract(random * 10.0));\n"
      "    }\n"
      "    return (0.9 + 0.2 * noise(pos)) * wallColor +\n"
      "           (0.9 + 0.2 * noise(pos)) * windowColor;\n"
      "\n"
      "  } else {\n"
      "    windowColor *= 0.08 * fract(10.0 * random);\n"
      "  }\n"
      "\n"
      "  return (0.9 + 0.2 * noise(pos)) * wallColor + windowColor;\n"
      "}\n"
      "\n"
      "vec3 textureRoof(vec2 pos, vec2 maxPos, vec2 squarer) {\n"
      "  float wsize = 0.025;\n"
      "  float randB = rand(squarer * 2.0);\n"
      "  vec3 wallColor = (0.3 + 1.4 * fract(randB * 100.0)) *\n"
      "                    vec3(0.1, 0.1, 0.1) +\n"
      "                   (-0.7 + 1.4 * fract(randB * 1000.0)) *\n"
      "                    vec3(0.02, 0., 0.);\n"
      "  vec3 conturColor = wallColor * 1.5 / 2.5;\n"
      "  vec2 contur = vec2(0.02);\n"
      "  if ((maxPos.x - pos.x < contur.x) ||\n"
      "      (maxPos.y - pos.y < contur.y) ||\n"
      "      (pos.x < contur.x) || (pos.y < contur.y)) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "  float step1 = 0.06 + 0.12 * fract(randB * 562526.2865);\n"
      "  pos -= step1;\n"
      "  maxPos -= step1 * 2.0;\n"
      "  if ((pos.x > 0.0 && pos.y > 0.0 && pos.x < maxPos.x &&\n"
      "       pos.y < maxPos.y) &&\n"
      "      ((abs(maxPos.x - pos.x) < contur.x) ||\n"
      "       (abs(maxPos.y - pos.y) < contur.y) ||\n"
      "       (abs(pos.x) < contur.x) ||\n"
      "       (abs(pos.y) < contur.y))) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "  pos -= step1;\n"
      "  maxPos -= step1 * 2.0;\n"
      "  if ((pos.x > 0.0 && pos.y > 0.0 && pos.x < maxPos.x &&\n"
      "       pos.y < maxPos.y) &&\n"
      "      ((abs(maxPos.x - pos.x) < contur.x) ||\n"
      "       (abs(maxPos.y - pos.y) < contur.y) ||\n"
      "       (abs(pos.x) < contur.x) ||\n"
      "       (abs(pos.y) < contur.y))) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "  pos -= step1;\n"
      "  maxPos -= step1 * 2.0;\n"
      "  if ((pos.x > 0.0 && pos.y > 0.0 && pos.x < maxPos.x &&\n"
      "       pos.y < maxPos.y) &&\n"
      "      ((abs(maxPos.x - pos.x) < contur.x) ||\n"
      "       (abs(maxPos.y - pos.y) < contur.y) ||\n"
      "       (abs(pos.x) < contur.x) ||\n"
      "       (abs(pos.y) < contur.y))) {\n"
      "    return (0.9 + 0.2 * noise(pos)) * conturColor;\n"
      "  }\n"
      "\n"
      "  return (0.9 + 0.2 * noise(pos)) * wallColor;\n"
      "}\n"
      "vec3 cars(vec2 squarer, vec2 pos, float dist, float level) {\n"
      "  vec3 color = vec3(0.0);\n"
      "  float carInten = 3.5 / sqrt(dist);\n"
      "  float carRadis = 0.01;\n"
      "  if (dist > 2.0) {\n"
      "    carRadis *= sqrt(dist / 2.0);\n"
      "  }\n"
      "  vec3 car1 = vec3(0.5, 0.5, 1.0);\n"
      "  vec3 car2 = vec3(1.0, 0.1, 0.1);\n"
      "  float carNumber = 0.5;\n"
      "\n"
      "  float random = noise((level + 1.0) * squarer * 1.24435824);\n"
      "  for (int j = 0; j < 10; j++) {\n"
      "    float i = 0.03 + float(j) * 0.094;\n"
      "    if (fract(random * 5.0 / i) > carNumber) {\n"
      "      color += car1 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(fract(i + iTime / 4.0),\n"
      "                          0.025)));\n"
      "    }\n"
      "\n"
      "    if (fract(random * 10.0 / i) > carNumber) {\n"
      "      color += car2 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(fract(i - iTime / 4.0),\n"
      "                          0.975)));\n"
      "    }\n"
      "    if (color.x > 0.0)\n"
      "      break;\n"
      "  }\n"
      "  for (int j = 0; j < 10; j++) {\n"
      "    float i = 0.03 + float(j) * 0.094;\n"
      "    if (fract(random * 5.0 / i) > carNumber) {\n"
      "      color += car2 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(0.025,\n"
      "                                            fract(i + iTime / 4.0))));\n"
      "    }\n"
      "    if (fract(random * 10.0 / i) > carNumber) {\n"
      "      color += car1 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(0.975,\n"
      "                                            fract(i - iTime / 4.0))));\n"
      "    }\n"
      "    if (color.x > 0.0)\n"
      "      break;\n"
      "  }\n"
      "  for (int j = 0; j < 10; j++) {\n"
      "    float i = 0.03 + 0.047 + float(j) * 0.094;\n"
      "    if (fract(random * 100.0 / i) > carNumber) {\n"
      "      color += car1 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(fract(i + iTime / 4.0),\n"
      "                                            0.045)));\n"
      "    }\n"
      "    if (fract(random * 1000.0 / i) > carNumber) {\n"
      "      color += car2 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(fract(i - iTime / 4.0),\n"
      "                                            0.955)));\n"
      "    }\n"
      "    if (color.x > 0.0)\n"
      "      break;\n"
      "  }\n"
      "  for (int j = 0; j < 10; j++) {\n"
      "    float i = 0.03 + 0.047 + float(j) * 0.094;\n"
      "    if (fract(random * 100.0 / i) > carNumber) {\n"
      "      color += car2 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(0.045,\n"
      "                                            fract(i + iTime / 4.0))));\n"
      "    }\n"
      "    if (fract(random * 1000.0 / i) > carNumber) {\n"
      "      color += car1 * carInten *\n"
      "               smoothstep(carRadis, 0.0,\n"
      "                          length(pos - vec2(0.955,\n"
      "                                            fract(i - iTime / 4.0))));\n"
      "    }\n"
      "    if (color.x > 0.0)\n"
      "      break;\n"
      "  }\n"
      "  return color;\n"
      "}\n"
      "vec3 textureGround(vec2 squarer, vec2 pos,\n"
      "                   vec2 vert1, vec2 vert2, float dist) {\n"
      "  vec3 color = (0.9 + 0.2 * noise(pos)) * vec3(0.1, 0.15, 0.1);\n"
      "  float randB = rand(squarer * 2.0);\n"
      "\n"
      "  vec3 wallColor = (0.3 + 1.4 * fract(randB * 100.0)) *\n"
      "                    vec3(0.1, 0.1, 0.1) +\n"
      "                   (-0.7 + 1.4 * fract(randB * 1000.0)) *\n"
      "                    vec3(0.02, 0., 0.);\n"
      "  float fund = 0.03;\n"
      "  float bl = 0.01;\n"
      "  float f = smoothstep(vert1.x - fund - bl, vert1.x - fund, pos.x);\n"
      "  f *= smoothstep(vert1.y - fund - bl, vert1.y - fund, pos.y);\n"
      "  f *= smoothstep(vert2.y + fund + bl, vert2.y + fund, pos.y);\n"
      "  f *= smoothstep(vert2.x + fund + bl, vert2.x + fund, pos.x);\n"
      "\n"
      "  pos -= 0.0;\n"
      "  vec2 maxPos = vec2(1., 1.);\n"
      "  vec2 contur = vec2(0.06, 0.06);\n"
      "  if ((pos.x > 0.0 && pos.y > 0.0 && pos.x < maxPos.x &&\n"
      "       pos.y < maxPos.y) &&\n"
      "      ((abs(maxPos.x - pos.x) < contur.x) ||\n"
      "       (abs(maxPos.y - pos.y) < contur.y) ||\n"
      "       (abs(pos.x) < contur.x) ||\n"
      "       (abs(pos.y) < contur.y))) {\n"
      "    color = vec3(0.1, 0.1, 0.1) * (0.9 + 0.2 * noise(pos));\n"
      "  }\n"
      "  pos -= 0.06;\n"
      "  maxPos = vec2(.88, 0.88);\n"
      "  contur = vec2(0.01, 0.01);\n"
      "  if ((pos.x > 0.0 && pos.y > 0.0 && pos.x < maxPos.x &&\n"
      "       pos.y < maxPos.y) &&\n"
      "      ((abs(maxPos.x - pos.x) < contur.x) ||\n"
      "       (abs(maxPos.y - pos.y) < contur.y) ||\n"
      "       (abs(pos.x) < contur.x) ||\n"
      "       (abs(pos.y) < contur.y))) {\n"
      "    color = vec3(0., 0., 0.);\n"
      "  }\n"
      "  color = mix(color, (0.9 + 0.2 * noise(pos)) * wallColor * 1.5 / 2.5,\n"
      "              f);\n"
      "\n"
      "  pos += 0.06;\n"
      "\n"
      "#ifdef CARS\n"
      "  if (pos.x < 0.07 || pos.x > 0.93 || pos.y < 0.07 || pos.y > 0.93) {\n"
      "    color += cars(squarer, pos, dist, 0.0);\n"
      "  }\n"
      "#endif\n"
      "\n"
      "  return color;\n"
      "}\n"
      "\n"
      "void main(void) {\n"
      "  vec2 pos = -1.0 + 2.0 * vUV.xy;\n"
      "  float t = -iTime;\n"
      "  float tt = -iTime - 0.5;\n"
      "\n"
      "  vec3 camPos =\n"
      "      vec3(5.0 + 12.0 * sin(t * 0.05), 5.0 + 7.0 * cos(t * 0.05),\n"
      "           1.9);\n"
      "\n"
      "  vec3 camTarget = vec3(5.0 + 0.0, 5.0 + 7.0 * sin(t * 0.05), 0.0);\n"
      "  if (fract(t / 12.0) < 0.25) {\n"
      "    camPos = vec3(5. * t, 3.1 * t, 2.1);\n"
      "    camTarget = vec3(5. * tt, 3.1 * tt, 1.7);\n"
      "  }\n"
      "  if (fract(t / 12.0) > 0.75) {\n"
      "    camPos = vec3(35., 3.1, 1.);\n"
      "    camTarget = vec3(35. + sin(t / 10.0), 3.1 + cos(t / 10.0), 0.7);\n"
      "  }\n"
      "  vec3 camDir = normalize(camTarget - camPos);\n"
      "  vec3 camUp = normalize(vec3(0.0, 0.0, -1.0));\n"
      "  vec3 camSide = cross(camDir, camUp);\n"
      "  camUp = cross(camDir, camSide);\n"
      "  vec3 rayDir = normalize(camSide * pos.x + camUp * pos.y +\n"
      "                          camDir * 1.6);\n"
      "  float angle = 0.03 * pow(abs(acos(rayDir.x)), 4.0);\n"
      "  // angle = min(0.0,angle);\n"
      "  vec3 color = vec3(0.0);\n"
      "  vec2 square = floor(camPos.xy);\n"
      "  square.x += 0.5 - 0.5 * sign(rayDir.x);\n"
      "  square.y += 0.5 - 0.5 * sign(rayDir.y);\n"
      "  float mind = 100.0;\n"
      "  int k = 0;\n"
      "  vec3 pol;\n"
      "  vec2 maxPos;\n"
      "  vec2 crossG;\n"
      "  float tSky = -(camPos.z - 3.9) / rayDir.z;\n"
      "  vec2 crossSky = floor(camPos.xy + rayDir.xy * tSky);\n"
      "  for (int i = 1; i < I_MAX; i++) {\n"
      "\n"
      "    vec2 squarer = square - vec2(0.5, 0.5) + 0.5 * sign(rayDir.xy);\n"
      "    if (crossSky == squarer && crossSky != floor(camPos.xy)) {\n"
      "      color += vec3(\n"
      "          vec2(0.5, 0.15) * abs(angle) * exp(-rayDir.z *\n"
      "          rayDir.z * 30.0), 0.2);\n"
      "      break;\n"
      "    }\n"
      "    float t;\n"
      "    float random = rand(squarer);\n"
      "    float height = 0.0;\n"
      "    float quartalR = rand(floor(squarer / 10.0));\n"
      "    if (floor(squarer / 10.0) == vec2(0.0, 0.0))\n"
      "      quartalR = 0.399;\n"
      "    if (quartalR < 0.4) {\n"
      "      height = -0.15 + 0.4 * random +\n"
      "               smoothstep(12.0, 7.0,\n"
      "                          length(fract(squarer / 10.0) * 10.0 -\n"
      "                                 vec2(5.0, 5.0))) *\n"
      "                   0.8 * random +\n"
      "               0.9 * smoothstep(10.0, 0.0,\n"
      "                                length(fract(squarer / 10.0) * 10.0 -\n"
      "                                       vec2(5.0, 5.0)));\n"
      "      height *= quartalR / 0.4;\n"
      "    }\n"
      "    float maxJ = 2.0;\n"
      "    float roof = 1.0;\n"
      "    if (height < 0.3) {\n"
      "      height = 0.3 * (0.7 + 1.8 * fract(random * 100.543264));\n"
      "      maxJ = 2.0;\n"
      "      if (fract(height * 1000.0) < 0.04)\n"
      "        height *= 1.3;\n"
      "    }\n"
      "    if (height > 0.5) {\n"
      "      maxJ = 3.0;\n"
      "    }\n"
      "    if (height > 0.85) {\n"
      "      maxJ = 4.0;\n"
      "    }\n"
      "    if (fract(height * 100.0) < 0.15) {\n"
      "      height = pow(maxJ - 1.0, 0.3) * height;\n"
      "      maxJ = 2.0;\n"
      "      roof = 0.0;\n"
      "    }\n"
      "\n"
      "    float maxheight = 1.5 * pow((maxJ - 1.0), 0.3) * height +\n"
      "                      roof * 0.07;\n"
      "    if (camPos.z +\n"
      "            rayDir.z *\n"
      "                (length(camPos.xy - square) + 0.71 - sign(rayDir.z) *\n"
      "                 0.71) /\n"
      "                length(rayDir.xy) <\n"
      "        maxheight) {\n"
      "      vec2 vert1r;\n"
      "      vec2 vert2r;\n"
      "      float zz = 0.0;\n"
      "      float prevZZ = 0.0;\n"
      "      for (int nf = 1; nf < 8; nf++) {\n"
      "        float j = float(nf);\n"
      "        if (j > maxJ) {\n"
      "          break;\n"
      "        }\n"
      "        prevZZ = zz;\n"
      "        zz = 1.5 * pow(j, 0.3) * height;\n"
      "        // prevZZ = zz-0.8;\n"
      "\n"
      "        float dia = 1.0 / pow(j, 0.3);\n"
      "        if (j == maxJ) {\n"
      "          if (roof == 0.0) {\n"
      "            break;\n"
      "          }\n"
      "          zz = 1.5 * pow((j - 1.0), 0.3) * height + 0.03 +\n"
      "               0.04 * fract(random * 1535.347);\n"
      "          dia = 1.0 / pow((j - 1.0), 0.3) - 0.2 - 0.2 *\n"
      "                fract(random * 10000.0);\n"
      "        }\n"
      "\n"
      "        vec2 v1 = vec2(0.0); // vec2(random*10.0,random*1.0);\n"
      "        vec2 v2 = vec2(0.0); // vec2(random*1000.0,random*100.0);\n"
      "        float randomF = fract(random * 10.0);\n"
      "        if (randomF < 0.25) {\n"
      "          v1 = vec2(fract(random * 1000.0), fract(random * 100.0));\n"
      "        }\n"
      "        if (randomF > 0.25 && randomF < 0.5) {\n"
      "          v1 = vec2(fract(random * 100.0), 0.0);\n"
      "          v2 = vec2(0.0, fract(random * 1000.0));\n"
      "        }\n"
      "        if (randomF > 0.5 && randomF < 0.75) {\n"
      "          v2 = vec2(fract(random * 1000.0), fract(random * 100.0));\n"
      "        }\n"
      "        if (randomF > 0.75) {\n"
      "          v1 = vec2(0.0, fract(random * 1000.0));\n"
      "          v2 = vec2(fract(random * 100.0), 0.0);\n"
      "        }\n"
      "        if (rayDir.y < 0.0) {\n"
      "          float y = v1.y;\n"
      "          v1.y = v2.y;\n"
      "          v2.y = y;\n"
      "        }\n"
      "        if (rayDir.x < 0.0) {\n"
      "          float x = v1.x;\n"
      "          v1.x = v2.x;\n"
      "          v2.x = x;\n"
      "        }\n"
      "\n"
      "        vec2 vert1 =\n"
      "            square + sign(rayDir.xy) * (0.5 - 0.37 *\n"
      "            (dia * 1.0 - 1.0 * v1));\n"
      "        vec2 vert2 =\n"
      "            square + sign(rayDir.xy) * (0.5 + 0.37 *\n"
      "            (dia * 1.0 - 1.0 * v2));\n"
      "        if (j == 1.0) {\n"
      "          vert1r = vec2(min(vert1.x, vert2.x), min(vert1.y, vert2.y));\n"
      "          vert2r = vec2(max(vert1.x, vert2.x), max(vert1.y, vert2.y));\n"
      "        }\n"
      "\n"
      "        vec3 pxy = polygonXY(zz, vert1, vert2, camPos, rayDir);\n"
      "        if (pxy.x < mind) {\n"
      "          mind = pxy.x;\n"
      "          pol = pxy;\n"
      "          k = 1;\n"
      "          maxPos = vec2(abs(vert1.x - vert2.x),\n"
      "                        abs(vert1.y - vert2.y));\n"
      "        }\n"
      "\n"
      "        vec3 pyz = polygonYZ(vert1.x, vec2(vert1.y, prevZZ),\n"
      "                             vec2(vert2.y, zz),\n"
      "                             camPos, rayDir);\n"
      "        if (pyz.x < mind) {\n"
      "          mind = pyz.x;\n"
      "          pol = pyz;\n"
      "          k = 2;\n"
      "          maxPos = vec2(abs(vert1.y - vert2.y), zz - prevZZ);\n"
      "        }\n"
      "\n"
      "        vec3 pxz = polygonXZ(vert1.y, vec2(vert1.x, prevZZ),\n"
      "                             vec2(vert2.x, zz),\n"
      "                             camPos, rayDir);\n"
      "        if (pxz.x < mind) {\n"
      "          mind = pxz.x;\n"
      "          pol = pxz;\n"
      "          k = 3;\n"
      "          maxPos = vec2(abs(vert1.x - vert2.x), zz - prevZZ);\n"
      "        }\n"
      "      }\n"
      "\n"
      "      if ((mind < 100.0) && (k == 1)) {\n"
      "        color += textureRoof(vec2(pol.y, pol.z), maxPos, squarer);\n"
      "        if (mind > 3.0) {\n"
      "          color *= sqrt(3.0 / mind);\n"
      "        }\n"
      "\n"
      "        break;\n"
      "      }\n"
      "      if ((mind < 100.0) && (k == 2)) {\n"
      "        color += textureWall(vec2(pol.y, pol.z),\n"
      "                             maxPos, squarer, 1.2075624928,\n"
      "                             height, mind, rayDir,\n"
      "                             vec3(1.0, 0.0, 0.0));\n"
      "        if (mind > 3.0) {\n"
      "          color *= sqrt(3.0 / mind);\n"
      "        }\n"
      "        break;\n"
      "      }\n"
      "\n"
      "      if ((mind < 100.0) && (k == 3)) {\n"
      "        color += textureWall(vec2(pol.y, pol.z), maxPos,\n"
      "                             squarer, 0.8093856205,\n"
      "                             height, mind, rayDir,\n"
      "                             vec3(0.0, 1.0, 0.0));\n"
      "        if (mind > 3.0) {\n"
      "          color *= sqrt(3.0 / mind);\n"
      "        }\n"
      "\n"
      "        break;\n"
      "      }\n"
      "      t = -camPos.z / rayDir.z;\n"
      "      crossG = camPos.xy + rayDir.xy * t;\n"
      "      if (floor(crossG) == squarer) {\n"
      "        mind = length(vec3(crossG, 0.0) - camPos);\n"
      "        color += textureGround(squarer, fract(crossG), fract(vert1r),\n"
      "                               fract(vert2r), mind);\n"
      "        if (mind > 3.0) {\n"
      "          color *= sqrt(3.0 / mind);\n"
      "        }\n"
      "\n"
      "        break;\n"
      "      }\n"
      "    }\n"
      "\n"
      "    if ((square.x + sign(rayDir.x) - camPos.x) / rayDir.x <\n"
      "        (square.y + sign(rayDir.y) - camPos.y) / rayDir.y) {\n"
      "      square.x += sign(rayDir.x) * 1.0;\n"
      "    } else {\n"
      "      square.y += sign(rayDir.y) * 1.0;\n"
      "    }\n"
      "\n"
      "    if (i == I_MAX - 1 && rayDir.z > -0.1) {\n"
      "      color += vec3(\n"
      "          vec2(0.5, 0.15) * abs(angle) * exp(-rayDir.z *\n"
      "               rayDir.z * 30.0), 0.2);\n"
      "    }\n"
      "  }\n"
      "  gl_FragColor = vec4(color, 1.0);\n"
      "}\n";

public:
  ShaderMaterialMorningCityScene(ICanvas* iCanvas)
      : IRenderableScene(iCanvas), _time{0.f}, _shaderMaterial{nullptr}
  {
    // Vertex shader
    Effect::ShadersStore()["customVertexShader"] = customVertexShader;

    // Fragment shader
    Effect::ShadersStore()["customFragmentShader"] = customFragmentShader;
  }

  ~ShaderMaterialMorningCityScene() override = default;

  const char* getName() override
  {
    return "Shader Material Morning City Scene";
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

}; // end of class ShaderMaterialMorningCityScene

BABYLON_REGISTER_SAMPLE("Shader Materials", ShaderMaterialMorningCityScene)

} // end of namespace Samples
} // end of namespace BABYLON
