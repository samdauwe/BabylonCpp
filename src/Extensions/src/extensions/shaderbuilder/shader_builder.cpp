#include <babylon/extensions/shaderbuilder/shader_builder.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/extensions/shaderbuilder/helper.h>
#include <babylon/extensions/shaderbuilder/icolor.h>
#include <babylon/extensions/shaderbuilder/ieffect.h>
#include <babylon/extensions/shaderbuilder/ilight.h>
#include <babylon/extensions/shaderbuilder/imap.h>
#include <babylon/extensions/shaderbuilder/inut.h>
#include <babylon/extensions/shaderbuilder/irange.h>
#include <babylon/extensions/shaderbuilder/ireflect_map.h>
#include <babylon/extensions/shaderbuilder/ireplace_color.h>
#include <babylon/extensions/shaderbuilder/shader.h>
#include <babylon/extensions/shaderbuilder/shader_material_helper_statics.h>

namespace BABYLON {
namespace Extensions {

ShaderBuilder::ShaderBuilder()
{
}

ShaderBuilder::~ShaderBuilder()
{
}

void ShaderBuilder::PrepareBeforeMaterialBuild()
{

  Attributes.emplace_back(ShaderMaterialHelperStatics::AttrPosition);
  Attributes.emplace_back(ShaderMaterialHelperStatics::AttrNormal);
  if (Setting.Uv) {
    Attributes.emplace_back(ShaderMaterialHelperStatics::AttrUv);
  }
  if (Setting.Uv2) {
    Attributes.emplace_back(ShaderMaterialHelperStatics::AttrUv2);
  }

  stl_util::concat(
    Uniforms,
    {std::string(ShaderMaterialHelperStatics::uniformView),
     std::string(ShaderMaterialHelperStatics::uniformWorld),
     std::string(ShaderMaterialHelperStatics::uniformWorldView),
     std::string(ShaderMaterialHelperStatics::uniformViewProjection),
     std::string(ShaderMaterialHelperStatics::uniformWorldViewProjection)});

  /** Start Build Vertex Frame **/
  Vertex.emplace_back("precision " + Setting.PrecisionMode + " float;");
  Vertex.emplace_back(
    "attribute " + std::string(ShaderMaterialHelperStatics::AttrTypeForPosition)
    + " " + std::string(ShaderMaterialHelperStatics::AttrPosition) + ";");
  Vertex.emplace_back(
    "attribute " + std::string(ShaderMaterialHelperStatics::AttrTypeForNormal)
    + " " + std::string(ShaderMaterialHelperStatics::AttrNormal) + ";");

  if (Setting.Uv) {
    Vertex.emplace_back(
      "attribute " + std::string(ShaderMaterialHelperStatics::AttrTypeForUv)
      + " " + std::string(ShaderMaterialHelperStatics::AttrUv) + ";");

    Vertex.emplace_back("varying vec2 "
                        + std::string(ShaderMaterialHelperStatics::Uv) + ";");
  }
  if (Setting.Uv2) {
    Vertex.emplace_back(
      "attribute " + std::string(ShaderMaterialHelperStatics::AttrTypeForUv2)
      + " " + std::string(ShaderMaterialHelperStatics::AttrUv2) + ";");

    Vertex.emplace_back("varying vec2 "
                        + std::string(ShaderMaterialHelperStatics::Uv2) + ";");
  }

  Vertex.emplace_back(
    "varying vec3 " + std::string(ShaderMaterialHelperStatics::Position) + ";");
  Vertex.emplace_back("varying vec3 "
                      + std::string(ShaderMaterialHelperStatics::Normal) + ";");

  Vertex.emplace_back(
    "uniform   " + std::string(ShaderMaterialHelperStatics::uniformStandardType)
    + " " + std::string(ShaderMaterialHelperStatics::uniformWorldViewProjection)
    + ";");
  if (Setting.VertexView) {
    Vertex.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformView) + ";");
  }

  if (Setting.VertexWorld) {
    Vertex.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformWorld) + ";");
  }

  if (Setting.VertexViewProjection) {
    Vertex.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformViewProjection) + ";");
  }

  if (Setting.Flags) {
    Uniforms.emplace_back(ShaderMaterialHelperStatics::uniformFlags);

    Vertex.emplace_back("uniform  float "
                        + std::string(ShaderMaterialHelperStatics::uniformFlags)
                        + ";");
  }

  if (Setting.VertexWorldView) {
    Vertex.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformWorldView) + ";");
  }

  if (!VertexUniforms.empty()) {
    Vertex.emplace_back(VertexUniforms);
  }

  /*#extension GL_OES_standard_derivatives : enable*/
  Fragment.emplace_back(
    "precision " + Setting.PrecisionMode
    + " float;\n#extension GL_OES_standard_derivatives : enable\n\n\n");

  if (Setting.Uv) {
    Fragment.emplace_back("varying vec2 "
                          + std::string(ShaderMaterialHelperStatics::Uv) + ";");
  }
  if (Setting.Uv2) {
    Fragment.emplace_back(
      "varying vec2 " + std::string(ShaderMaterialHelperStatics::Uv2) + ";");
  }

  if (Setting.FragmentView) {
    Fragment.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformView) + ";");
  }

  if (Setting.FragmentWorld) {
    Fragment.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformWorld) + ";");
  }

  if (Setting.FragmentViewProjection) {
    Fragment.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformViewProjection) + ";");
  }

  if (Setting.FragmentWorldView) {
    Fragment.emplace_back(
      "uniform   "
      + std::string(ShaderMaterialHelperStatics::uniformStandardType) + " "
      + std::string(ShaderMaterialHelperStatics::uniformWorldView) + ";");
  }

  if (Setting.Flags) {
    Fragment.emplace_back(
      "uniform  float " + std::string(ShaderMaterialHelperStatics::uniformFlags)
      + ";");
  }

  if (!FragmentUniforms.empty()) {
    Fragment.emplace_back(FragmentUniforms);
  }
  Fragment.emplace_back(
    "varying vec3 " + std::string(ShaderMaterialHelperStatics::Position) + ";");
  Fragment.emplace_back(
    "varying vec3 " + std::string(ShaderMaterialHelperStatics::Normal) + ";");

  if (Setting.WorldPosition) {
    Vertex.emplace_back(
      "varying vec3 " + std::string(ShaderMaterialHelperStatics::WorldPosition)
      + ";");
    Vertex.emplace_back("varying vec3 "
                        + std::string(ShaderMaterialHelperStatics::WorldNormal)
                        + ";");

    Fragment.emplace_back(
      "varying vec3 " + std::string(ShaderMaterialHelperStatics::WorldPosition)
      + ";");
    Fragment.emplace_back(
      "varying vec3 " + std::string(ShaderMaterialHelperStatics::WorldNormal)
      + ";");
  }

  if (!Setting.Texture2Ds.empty()) {
    unsigned int cnt = 0;
    for (auto& texture2D : Setting.Texture2Ds) {
      std::string s = std::to_string(cnt);
      if (texture2D.inVertex) {
        Vertex.emplace_back(
          "uniform  sampler2D "
          + std::string(ShaderMaterialHelperStatics::Texture2D) + s + ";");
      }
      if (texture2D.inFragment) {
        Fragment.emplace_back(
          "uniform  sampler2D  "
          + std::string(ShaderMaterialHelperStatics::Texture2D) + s + ";");
      }
      ++cnt;
    }
  }

  if (Setting.CameraShot) {
    Fragment.emplace_back("uniform  sampler2D  textureSampler;");
  }

  if (!Setting.TextureCubes.empty()) {
    unsigned int cnt = 0;
    for (auto& textureCube : Setting.TextureCubes) {
      std::string s = std::to_string(cnt);
      if (textureCube.inVertex) {
        Vertex.emplace_back(
          "uniform  samplerCube  "
          + std::string(ShaderMaterialHelperStatics::TextureCube) + s + ";");
      }
      if (textureCube.inFragment) {
        Fragment.emplace_back(
          "uniform  samplerCube   "
          + std::string(ShaderMaterialHelperStatics::TextureCube) + s + ";");
      }
      ++cnt;
    }
  }

  if (Setting.Center) {
    Vertex.emplace_back("uniform  vec3 "
                        + std::string(ShaderMaterialHelperStatics::Center)
                        + ";");
    Fragment.emplace_back("uniform  vec3 "
                          + std::string(ShaderMaterialHelperStatics::Center)
                          + ";");
  }
  if (Setting.Mouse) {
    Vertex.emplace_back(
      "uniform  vec2 " + std::string(ShaderMaterialHelperStatics::Mouse) + ";");
    Fragment.emplace_back(
      "uniform  vec2 " + std::string(ShaderMaterialHelperStatics::Mouse) + ";");
  }
  if (Setting.Screen) {
    Vertex.emplace_back("uniform  vec2 "
                        + std::string(ShaderMaterialHelperStatics::Screen)
                        + ";");
    Fragment.emplace_back("uniform  vec2 "
                          + std::string(ShaderMaterialHelperStatics::Screen)
                          + ";");
  }
  if (Setting.Camera) {
    Vertex.emplace_back("uniform  vec3 "
                        + std::string(ShaderMaterialHelperStatics::Camera)
                        + ";");
    Fragment.emplace_back("uniform  vec3 "
                          + std::string(ShaderMaterialHelperStatics::Camera)
                          + ";");
  }
  if (Setting.Look) {
    Vertex.emplace_back("uniform  vec3 "
                        + std::string(ShaderMaterialHelperStatics::Look) + ";");
    Fragment.emplace_back(
      "uniform  vec3 " + std::string(ShaderMaterialHelperStatics::Look) + ";");
  }
  if (Setting.Time) {
    Vertex.emplace_back("uniform  float "
                        + std::string(ShaderMaterialHelperStatics::Time) + ";");
    Fragment.emplace_back(
      "uniform  float " + std::string(ShaderMaterialHelperStatics::Time) + ";");
  }
  if (Setting.GlobalTime) {
    Vertex.emplace_back("uniform  vec4 "
                        + std::string(ShaderMaterialHelperStatics::GlobalTime)
                        + ";");
    Fragment.emplace_back("uniform  vec4 "
                          + std::string(ShaderMaterialHelperStatics::GlobalTime)
                          + ";");
  }
  if (Setting.ReflectMatrix) {
    Vertex.emplace_back(
      "uniform  mat4 " + std::string(ShaderMaterialHelperStatics::ReflectMatrix)
      + ";");
    Fragment.emplace_back(
      "uniform  mat4 " + std::string(ShaderMaterialHelperStatics::ReflectMatrix)
      + ";");
  }
  if (Setting.Helpers) {
    constexpr const char* helpers
      = "vec3 random3(vec3 c) {\n"
        "  float j = 4096.0 * sin(dot(c, vec3(17.0, 59.4, 15.0)));\n"
        "  vec3 r;\n"
        "  r.z = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.x = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.y = fract(512.0 * j);\n"
        "  return r - 0.5;\n"
        "}\n"
        "float rand(vec2 co) {\n"
        "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
        "}\n"
        "const float F3 = 0.3333333;\n"
        "const float G3 = 0.1666667;\n"
        "float simplex3d(vec3 p) {\n"
        "  vec3 s = floor(p + dot(p, vec3(F3)));\n"
        "  vec3 x = p - s + dot(s, vec3(G3));\n"
        "  vec3 e = step(vec3(0.0), x - x.yzx);\n"
        "  vec3 i1 = e * (1.0 - e.zxy);\n"
        "  vec3 i2 = 1.0 - e.zxy * (1.0 - e);\n"
        "  vec3 x1 = x - i1 + G3;\n"
        "  vec3 x2 = x - i2 + 2.0 * G3;\n"
        "  vec3 x3 = x - 1.0 + 3.0 * G3;\n"
        "  vec4 w, d;\n"
        "  w.x = dot(x, x);\n"
        "  w.y = dot(x1, x1);\n"
        "  w.z = dot(x2, x2);\n"
        "  w.w = dot(x3, x3);\n"
        "  w = max(0.6 - w, 0.0);\n"
        "  d.x = dot(random3(s), x);\n"
        "  d.y = dot(random3(s + i1), x1);\n"
        "  d.z = dot(random3(s + i2), x2);\n"
        "  d.w = dot(random3(s + 1.0), x3);\n"
        "  w *= w;\n"
        "  w *= w;\n"
        "  d *= w;\n"
        "  return dot(d, vec4(52.0));\n"
        "}\n"
        "float noise(vec3 m) {\n"
        "  return 0.5333333 * simplex3d(m) +\n"
        "         0.2666667 * simplex3d(2.0 * m) +\n"
        "         0.1333333 * simplex3d(4.0 * m) +\n"
        "         0.0666667 * simplex3d(8.0 * m);\n"
        "}\n"
        "float dim(vec3 p1, vec3 p2) {\n"
        "  return sqrt((p2.x - p1.x) * (p2.x - p1.x) +\n"
        "              (p2.y - p1.y) * (p2.y - p1.y) +\n"
        "              (p2.z - p1.z) * (p2.z - p1.z));\n"
        "}\n"
        "vec2 rotate_xy(vec2 pr1, vec2 pr2, float alpha) {\n"
        "  vec2 pp2 = vec2(pr2.x - pr1.x, pr2.y - pr1.y);\n"
        "  return vec2(pr1.x + pp2.x * cos(alpha * 3.14159265 / 180.) -\n"
        "                  pp2.y * sin(alpha * 3.14159265 / 180.),\n"
        "              pr1.y + pp2.x * sin(alpha * 3.14159265 / 180.) +\n"
        "                  pp2.y * cos(alpha * 3.14159265 / 180.));\n"
        "}\n"
        "vec3 r_y(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.x;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.z), a);\n"
        "  n.x = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_x(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.y;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.y, n.z), a);\n"
        "  n.y = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_z(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.y), a);\n"
        "  n.x = p.x;\n"
        "  n.y = p.y;\n"
        "  return n;\n"
        "}\n";
    Vertex.emplace_back(helpers);
    Fragment.emplace_back(helpers);
  }

  std::ostringstream oss;
  oss << "void main(void) {\n";
  oss << ShaderMaterialHelperStatics::Position << " = ";
  oss << ShaderMaterialHelperStatics::AttrPosition << ";\n";
  oss << ShaderMaterialHelperStatics::Normal << " = ";
  oss << ShaderMaterialHelperStatics::AttrNormal << ";\n";
  oss << "vec4 result = vec4(";
  oss << std::string(ShaderMaterialHelperStatics::Position) + ",1.);\n";
  oss << "vuv = uv;\n";
  oss << "#[Source]\n";
  oss << "gl_Position = worldViewProjection * result;\n";
  oss << "#[AfterFinishVertex]\n";
  oss << "}";
  Vertex.emplace_back(oss.str());

  /** start Build Fragment Frame **/
  if (!Setting.NormalMap.empty()) {
    oss.str(std::string());
    oss << "vec3 normalMap() { vec4 result = vec4(0.); ";
    oss << Setting.NormalMap << ";\n";
    oss << "result = vec4( normalize( " << Setting.Normal;
    oss << " -(normalize(result.xyz)*2.0-vec3(1.))*(max(-0.5,min(0.5,";
    oss << Shader::Print(Setting.NormalOpacity);
    oss << ")) )),1.0); return result.xyz;}";
    Fragment.emplace_back(oss.str());
  }

  if (!Setting.SpecularMap.empty()) {
    oss.str(std::string());
    oss << "float specularMap() { ";
    oss << "vec4 result = vec4(0.);float float_result = 0.; ";
    oss << Setting.SpecularMap << ";";
    oss << "return float_result ;}";
    Fragment.emplace_back(oss.str());
  }

  Fragment.emplace_back(FragmentBeforeMain);

  oss.str(std::string());
  oss << "void main(void) { \n";
  oss << "   int discardState = 0;\n";
  oss << "   vec4 result = vec4(0.);\n";
  oss << "   #[Source] \n";
  oss << "   if(discardState == 0) gl_FragColor = result; \n";
  oss << "}";
  Fragment.emplace_back(oss.str());
}

void ShaderBuilder::PrepareBeforePostProcessBuild()
{
  Attributes.emplace_back(ShaderMaterialHelperStatics::AttrPosition);

  /** Start Build Vertex Frame **/
  /*#extension GL_OES_standard_derivatives : enable*/
  Fragment.emplace_back("precision " + Setting.PrecisionMode + " float;\n");

  if (Setting.Uv) {
    Fragment.emplace_back("varying vec2 vUV;");
  }

  if (Setting.Flags) {
    Fragment.emplace_back(
      "uniform float " + std::string(ShaderMaterialHelperStatics::uniformFlags)
      + ";");
  }

  if (!Setting.Texture2Ds.empty()) {
    unsigned int cnt = 0;
    for (auto& texture2D : Setting.Texture2Ds) {
      std::string s = std::to_string(cnt);
      if (texture2D.inFragment) {
        Fragment.emplace_back(
          "uniform sampler2D "
          + std::string(ShaderMaterialHelperStatics::Texture2D) + s + ";");
      }
      ++cnt;
    }
  }

  if (!PPSSamplers.empty()) {
    for (auto& PPSSampler : PPSSamplers) {
      if (!PPSSampler.empty()) {
        Fragment.emplace_back("uniform sampler2D  " + PPSSampler + ";");
      }
    }
  }

  if (Setting.CameraShot) {
    Fragment.emplace_back("uniform sampler2D textureSampler;");
  }

  if (Setting.Mouse) {
    Fragment.emplace_back(
      "uniform vec2 " + std::string(ShaderMaterialHelperStatics::Mouse) + ";");
  }
  if (Setting.Screen) {
    Fragment.emplace_back(
      "uniform vec2 " + std::string(ShaderMaterialHelperStatics::Screen) + ";");
  }
  if (Setting.Camera) {
    Fragment.emplace_back(
      "uniform vec3 " + std::string(ShaderMaterialHelperStatics::Camera) + ";");
  }
  if (Setting.Look) {
    Fragment.emplace_back(
      "uniform vec3 " + std::string(ShaderMaterialHelperStatics::Look) + ";");
  }
  if (Setting.Time) {
    Fragment.emplace_back(
      "uniform float " + std::string(ShaderMaterialHelperStatics::Time) + ";");
  }
  if (Setting.GlobalTime) {
    Fragment.emplace_back("uniform vec4 "
                          + std::string(ShaderMaterialHelperStatics::GlobalTime)
                          + ";");
  }

  if (Setting.Helpers) {
    constexpr const char* helpers
      = "vec3 random3(vec3 c) {\n"
        "  float j = 4096.0 * sin(dot(c, vec3(17.0, 59.4, 15.0)));\n"
        "  vec3 r;\n"
        "  r.z = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.x = fract(512.0 * j);\n"
        "  j *= .125;\n"
        "  r.y = fract(512.0 * j);\n"
        "  return r - 0.5;\n"
        "}\n"
        "float rand(vec2 co) {\n"
        "  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);\n"
        "}\n"
        "const float F3 = 0.3333333;\n"
        "const float G3 = 0.1666667;\n"
        "float simplex3d(vec3 p) {\n"
        "  vec3 s = floor(p + dot(p, vec3(F3)));\n"
        "  vec3 x = p - s + dot(s, vec3(G3));\n"
        "  vec3 e = step(vec3(0.0), x - x.yzx);\n"
        "  vec3 i1 = e * (1.0 - e.zxy);\n"
        "  vec3 i2 = 1.0 - e.zxy * (1.0 - e);\n"
        "  vec3 x1 = x - i1 + G3;\n"
        "  vec3 x2 = x - i2 + 2.0 * G3;\n"
        "  vec3 x3 = x - 1.0 + 3.0 * G3;\n"
        "  vec4 w, d;\n"
        "  w.x = dot(x, x);\n"
        "  w.y = dot(x1, x1);\n"
        "  w.z = dot(x2, x2);\n"
        "  w.w = dot(x3, x3);\n"
        "  w = max(0.6 - w, 0.0);\n"
        "  d.x = dot(random3(s), x);\n"
        "  d.y = dot(random3(s + i1), x1);\n"
        "  d.z = dot(random3(s + i2), x2);\n"
        "  d.w = dot(random3(s + 1.0), x3);\n"
        "  w *= w;\n"
        "  w *= w;\n"
        "  d *= w;\n"
        "  return dot(d, vec4(52.0));\n"
        "}\n"
        "float noise(vec3 m) {\n"
        "  return 0.5333333 * simplex3d(m) +\n"
        "         0.2666667 * simplex3d(2.0 * m) +\n"
        "         0.1333333 * simplex3d(4.0 * m) +\n"
        "         0.0666667 * simplex3d(8.0 * m);\n"
        "}\n"
        "vec2 rotate_xy(vec2 pr1, vec2 pr2, float alpha) {\n"
        "  vec2 pp2 = vec2(pr2.x - pr1.x, pr2.y - pr1.y);\n"
        "  return vec2(pr1.x + pp2.x * cos(alpha * 3.14159265 / 180.) -\n"
        "                  pp2.y * sin(alpha * 3.14159265 / 180.),\n"
        "              pr1.y + pp2.x * sin(alpha * 3.14159265 / 180.) +\n"
        "                  pp2.y * cos(alpha * 3.14159265 / 180.));\n"
        "}\n"
        "vec3 r_y(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.x;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.z), a);\n"
        "  n.x = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_x(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  c1.x = c1.y;\n"
        "  c1.y = c1.z;\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.y, n.z), a);\n"
        "  n.y = p.x;\n"
        "  n.z = p.y;\n"
        "  return n;\n"
        "}\n"
        "vec3 r_z(vec3 n, float a, vec3 c) {\n"
        "  vec3 c1 = vec3(c.x, c.y, c.z);\n"
        "  vec2 p = rotate_xy(vec2(c1.x, c1.y), vec2(n.x, n.y), a);\n"
        "  n.x = p.x;\n"
        "  n.y = p.y;\n"
        "  return n;\n"
        "}\n"
        "float getIdColor(vec4 a) {\n"
        "  float b = 255.;\n"
        "  float c = 255. / b;\n"
        "  float x = floor(a.x * 256. / c);\n"
        "  float y = floor(a.y * 256. / c);\n"
        "  float z = floor(a.z * 256. / c);\n"
        "  return z * b * b + y * b + x;\n"
        "}\n";
    Fragment.emplace_back(helpers);
  }

  std::ostringstream oss;
  if (!Setting.NormalMap.empty()) {
    oss.str(std::string());
    oss << "vec3 normalMap() { ";
    oss << "vec4 result = vec4(0.);";
    oss << "return result.xyz;}";
    Fragment.emplace_back(oss.str());
  }

  /** start Build Fragment Frame **/
  Fragment.emplace_back(FragmentBeforeMain);

  oss.str(std::string());
  oss << "void main(void) { \n";
  oss << "   int discardState = 0;\n";
  oss << "   vec2 vuv = vUV;\n";
  oss << "   vec3 center = vec3(0.);\n";
  oss << "   vec4 result = vec4(0.);\n";
  oss << "   #[Source] \n";
  oss << "   if(discardState == 0) gl_FragColor = result; \n";
  oss << "}";
  Fragment.emplace_back(oss.str());
}

ShaderMaterial* ShaderBuilder::PrepareMaterial(ShaderMaterial* /*material*/,
                                               Scene* /*scene*/)
{
  return nullptr;
}

std::string ShaderBuilder::Build()
{
  Shader::Me->Parent->Setting = Shader::Me->Setting;
  Shader::Me                  = Shader::Me->Parent;

  return Body;
}

std::string ShaderBuilder::BuildVertex()
{
  Shader::Me->Parent->Setting = Shader::Me->Setting;
  Shader::Me                  = Shader::Me->Parent;

  return VertexBody;
}

ShaderBuilder& ShaderBuilder::SetUniform(const std::string& name,
                                         const std::string& type)
{
  if (Shader::Me->VertexUniforms.empty()) {
    Shader::Me->VertexUniforms = "";
  }
  if (Shader::Me->FragmentUniforms.empty()) {
    Shader::Me->FragmentUniforms = "";
  }

  VertexUniforms += "uniform " + type + " " + name + ";\n";
  FragmentUniforms += "uniform " + type + " " + name + ";\n";

  return *this;
}

ShaderMaterial* ShaderBuilder::BuildMaterial(Scene* /*scene*/)
{
  return nullptr;
}

PostProcess* ShaderBuilder::BuildPostProcess(Camera* /*camera*/,
                                             Scene* /*scene*/, float /*scale*/,
                                             const std::string& /*option*/)
{
  Setting.Screen     = true;
  Setting.Mouse      = true;
  Setting.Time       = true;
  Setting.CameraShot = true;

  PrepareBeforePostProcessBuild();

  ++Shader::ShaderIdentity;
  std::vector<std::string> samplers;
  for (size_t s = 0; s < Setting.Texture2Ds.size(); ++s) {
    samplers.emplace_back(ShaderMaterialHelperStatics::Texture2D
                          + std::to_string(s));
  }

  if (!PPSSamplers.empty()) {
    for (auto& PPSSampler : PPSSamplers) {
      if (!PPSSampler.empty()) {
        samplers.emplace_back(PPSSampler);
      }
    }
  }

  return nullptr;
}

ShaderBuilder& ShaderBuilder::Event(int index, const std::string& mat)
{
  Shader::Me->Setting.Flags = true;

  ++Shader::Indexer;

  std::ostringstream oss;
  oss << Shader::Def(Body, "");
  oss << "  if ( floor(mod( " << ShaderMaterialHelperStatics::uniformFlags
      << "/pow(2.," << Shader::Print(index) << "),2.)) == 1.) { " << mat
      << " } ";

  Body = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::EventVertex(int index, const std::string& mat)
{
  Shader::Me->Setting.Flags  = true;
  Shader::Me->Setting.Vertex = true;
  ++Shader::Indexer;

  std::ostringstream oss;
  oss << Shader::Def(VertexBody, "");
  oss << " if( floor(mod( " << ShaderMaterialHelperStatics::uniformFlags
      << "/pow(2.," << Shader::Print(index) << "),2.)) == 1. ){ " << mat << "}";

  VertexBody = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::Transparency()
{
  Shader::Me->Setting.Transparency = true;

  return *this;
}

ShaderBuilder& ShaderBuilder::PostEffect1(size_t id, const std::string& effect)
{
  if (id >= Shader::Me->PostEffect1Effects.size()) {
    Shader::Me->PostEffect1Effects.resize(id + 1);
  }
  Shader::Me->PostEffect1Effects[id] = effect;

  return *this;
}

ShaderBuilder& ShaderBuilder::PostEffect2(size_t id, const std::string& effect)
{
  if (id >= Shader::Me->PostEffect2Effects.size()) {
    Shader::Me->PostEffect2Effects.resize(id + 1);
  }
  Shader::Me->PostEffect2Effects[id] = effect;

  return *this;
}

ShaderBuilder&
ShaderBuilder::ImportSamplers(const std::vector<std::string>& txts)
{
  stl_util::concat(Shader::Me->PPSSamplers, txts);

  return *this;
}

ShaderBuilder& ShaderBuilder::Wired()
{
  Shader::Me->Setting.Wire = true;

  return *this;
}

ShaderBuilder& ShaderBuilder::VertexShader(const std::string& mat)
{
  VertexBody = Shader::Def(VertexBody, "");
  VertexBody += mat;

  return *this;
}

ShaderBuilder& ShaderBuilder::Solid(const IColor& iColor)
{
  IColor color;
  color.a = Shader::Def(iColor.a, 1.f);
  color.r = Shader::Def(iColor.r, 0.f);
  color.g = Shader::Def(iColor.g, 0.f);
  color.b = Shader::Def(iColor.b, 0.f);

  std::ostringstream oss;
  oss << Shader::Def(Body, "");
  oss << " result = vec4(" << Shader::Print(color.r) << ","
      << Shader::Print(color.g) << "," << Shader::Print(color.b) << ","
      << Shader::Print(color.a) << ");";

  Body = oss.str();

  return *this;
}

int ShaderBuilder::GetMapIndex(const std::string key)
{
  auto it = std::find_if(
    Shader::Me->Setting.Texture2Ds.begin(),
    Shader::Me->Setting.Texture2Ds.end(),
    [&key](const ITexture& texture) { return texture.key == key; });

  return (it == Shader::Me->Setting.Texture2Ds.end()) ?
           -1 :
           static_cast<int>(it - Shader::Me->Setting.Texture2Ds.begin());
}

int ShaderBuilder::GetCubeMapIndex(const std::string key)
{
  auto it = std::find_if(
    Shader::Me->Setting.TextureCubes.begin(),
    Shader::Me->Setting.TextureCubes.end(),
    [&key](const ITexture& texture) { return texture.key == key; });

  return (it == Shader::Me->Setting.TextureCubes.end()) ?
           -1 :
           static_cast<int>(it - Shader::Me->Setting.TextureCubes.begin());
}

std::string Nut(std::string& /*value*/, const INut& /*option*/)
{
  return "";
}

ShaderBuilder& ShaderBuilder::Map(IMap& option)
{
  std::string refInd;

  Shader::Me->Setting.Center  = true;
  Shader::Me->Setting.Helpers = true;
  Shader::Me->Setting.Uv      = true;

  float frameLength
    = std::min(option.animationFrameEnd - option.animationFrameStart,
               static_cast<float>(option.indexCount * option.indexCount));

  std::string uv = Shader::Def(option.uv, ShaderMaterialHelperStatics::Uv);

  if (option.uv == "planar") {
    uv = ShaderMaterialHelperStatics::Position;
  }
  else {
    uv = "vec3(" + option.uv + ".x," + option.uv + ".y,0.)";
  }

  option.scaleX /= option.indexCount;
  option.scaleY /= option.indexCount;

  // rotate
  std::ostringstream oss;
  oss << "vec3 centeri#[Ind] = " << ShaderMaterialHelperStatics::Center << ";\n"
      << "vec3 ppo#[Ind] = r_z( " << uv
      << "," + Shader::Print(option.rotation.x) << ",centeri#[Ind]);\n"
      << " ppo#[Ind] = r_y( ppo#[Ind]," << Shader::Print(option.rotation.y)
      << ",centeri#[Ind]);\n"
      << " ppo#[Ind] = r_x( ppo#[Ind]," << Shader::Print(option.rotation.x)
      << ",centeri#[Ind]);\n"
      << "vec3 nrm#[Ind] = r_z( " << option.normal + ","
      << Shader::Print(option.rotation.x) << ",centeri#[Ind]);\n"
      << " nrm#[Ind] = r_y( nrm#[Ind]," << Shader::Print(option.rotation.y)
      << ",centeri#[Ind]);\n"
      << " nrm#[Ind] = r_x( nrm#[Ind]," << Shader::Print(option.rotation.z)
      << ",centeri#[Ind]);\n";

  // sresult
  oss << "vec4 color#[Ind] = texture2D(" << refInd << " ,ppo#[Ind].xy*vec2("
      << Shader::Print(option.scaleX) << "," << Shader::Print(option.scaleY)
      << ")+vec2(" << Shader::Print(option.x) << "," << Shader::Print(option.y)
      << ")" << (option.bias.empty() || Shader::Print(option.bias) == "0." ?
                   "" :
                   "," + Shader::Print(option.bias))
      << ");\n"
      << "if(nrm#[Ind].z < " + Shader::Print(option.normalLevel) + "){\n"
      << (option.alpha ? " result =  color#[Ind];" :
                         "result = vec4(color#[Ind].rgb , 1.); ")
      << "\n"
      << "}\n";

  if (option.indexCount > 1 || option.tiled) {
    option.columnIndex = option.indexCount - option.columnIndex + 1;

    oss.str(std::string());
    oss << " vec3 uvt#[Ind] = vec3(" << uv << ".x*"
        << Shader::Print(option.scaleX) << "+" << Shader::Print(option.x) << ","
        << uv << ".y*" << Shader::Print(option.scaleY) << "+"
        << Shader::Print(option.y) << ",0.0);\n"
        << "\n"
        << " float xst#[Ind] = 1./(" << Shader::Print(option.indexCount)
        << "*2.);\n"
        << " float yst#[Ind] =1./(" << Shader::Print(option.indexCount)
        << "*2.);\n"
        << " float xs#[Ind] = 1./" << Shader::Print(option.indexCount) << ";\n"
        << " float ys#[Ind] = 1./" << Shader::Print(option.indexCount) << ";\n"
        << " float yid#[Ind] = " << Shader::Print(option.columnIndex - 1.f)
        << ";\n"
        << " float xid#[Ind] = " << Shader::Print(option.rowIndex - 1.f)
        << ";\n"
        << (option.animation ?
              " float ind_a#[Ind] = floor(mod(time*0.001*"
                + Shader::Print(option.animationSpeed) + ", "
                + Shader::Print(frameLength) + " )+"
                + Shader::Print(option.animationFrameStart) + ");"
                + " yid#[Ind] = " + Shader::Print(option.indexCount)
                + "- floor(ind_a#[Ind] / " + Shader::Print(option.indexCount)
                + ");" //
                + " xid#[Ind] =  floor(mod(ind_a#[Ind] ,  "
                + Shader::Print(option.indexCount) + ")); " :
              "")
        << "\n"
        << " float xi#[Ind] = mod(uvt#[Ind].x ,xs#[Ind])+xs#[Ind]*xid#[Ind];\n"
        << " float yi#[Ind] = mod(uvt#[Ind].y ,ys#[Ind])+ys#[Ind]*yid#[Ind];\n"
        << "\n"
        << " float xi2#[Ind] = mod(uvt#[Ind].x - "
        << "xs#[Ind]*0.5,xs#[Ind])+xs#[Ind]*xid#[Ind];\n"
        << " float yi2#[Ind] = mod(uvt#[Ind].y - "
        << "ys#[Ind]*0.5,ys#[Ind])+ys#[Ind]*yid#[Ind];\n"
        << "\n\n"
        << " vec4 f#[Ind] = texture2D(" << refInd
        << ",vec2(xi#[Ind],yi#[Ind]));\n"
        << " result = f#[Ind];\n";

    std::ostringstream ossTiled;
    ossTiled
      << " vec4 f2#[Ind] = texture2D(" << refInd
      << ",vec2(xi2#[Ind]+xid#[Ind] ,yi#[Ind]));\n"
      << " vec4 f3#[Ind] = texture2D(" << refInd
      << ",vec2(xi#[Ind],yi2#[Ind]+yid#[Ind]));\n"
      << " vec4 f4#[Ind] = texture2D(" << refInd
      << ",vec2(xi2#[Ind]+xid#[Ind],yi2#[Ind]+yid#[Ind]));\n"
      << "\n\n"
      << " float ir#[Ind] = 0.,ir2#[Ind] = 0.;\n"
      << "\n"
      << "     if( yi2#[Ind]  >= yid#[Ind] *ys#[Ind] ){\n"
      << "         ir2#[Ind]  = min(2.,max(0.,( yi2#[Ind]-yid#[Ind] "
      << "*ys#[Ind])*2.0/ys#[Ind] ));\n"
      << "         if(ir2#[Ind] > 1.0) ir2#[Ind] =1.0-(ir2#[Ind]-1.0);\n"
      << "         ir2#[Ind] = min(1.0,max(0.0,pow(ir2#[Ind],"
      << Shader::Print(15.f) + " )*" << Shader::Print(3.f) << "));\n"
      << "         result =  result *(1.0-ir2#[Ind]) +f3#[Ind]*ir2#[Ind];\n"
      << "     }\n"
      << " if( xi2#[Ind]  >= xid#[Ind] *xs#[Ind] ){\n"
      << "         ir2#[Ind]  = min(2.,max(0.,( xi2#[Ind]-xid#[Ind] "
      << "*xs#[Ind])*2.0/xs#[Ind] ));\n"
      << "         if(ir2#[Ind] > 1.0) ir2#[Ind] =1.0-(ir2#[Ind]-1.0);\n"
      << "         ir2#[Ind] = min(1.0,max(0.0,pow(ir2#[Ind],"
      << Shader::Print(15.f) << " )*" + Shader::Print(3.f) + "));\n"
      << "         result = result*(1.0-ir2#[Ind])+f2#[Ind]*ir2#[Ind];\n"
      << "     }\n"
      << " if( xi2#[Ind]  >= xid#[Ind]*xs#[Ind] && xi2#[Ind] >= "
      << "xid#[Ind]*xs#[Ind]){\n"
      << "         ir2#[Ind] = min(2.,max(0.,( xi2#[Ind]-xid#[Ind] "
      << "*xs#[Ind])*2.0/xs#[Ind] ));\n"
      << "         float ir3#[Ind] = min(2.,max(0.,( yi2#[Ind]-yid#[Ind] "
      << "*ys#[Ind])*2.0/ys#[Ind] ));\n"
      << "         if(ir2#[Ind] > 1.0) ir2#[Ind] = 1.0-(ir2#[Ind]-1.0);\n"
      << "         if(ir3#[Ind] > 1.0) ir3#[Ind] = 1.0-(ir3#[Ind]-1.0);\n"
      << "         ir2#[Ind] = min(1.0,max(0.0,pow(ir2#[Ind],"
      << Shader::Print(15.f) + " )*" + Shader::Print(3.f) + "));\n"
      << "         ir3#[Ind] = min(1.0,max(0.0,pow(ir3#[Ind],"
      << Shader::Print(15.f) + " )*" + Shader::Print(3.f) + "));\n"
      << "         ir2#[Ind] = min(1.0,max(0.0, ir2#[Ind]* ir3#[Ind] ));\n"
      << " if(nrm#[Ind].z < " + Shader::Print(option.normalLevel) + "){\n"
      << (option.alpha ?
            "    result = result *(1.0-ir2#[Ind]) +f4#[Ind]* ir2#[Ind];" :
            "    result = vec4(result.xyz*(1.0-ir2#[Ind])+f4#[Ind].xyz* "
            "ir2#[Ind],1.0);")
      << "\n"
      << "}\n"
      << "}\n";

    oss << (option.tiled ? ossTiled.str() : "");
    oss << "\n";
  }

  Body = Shader::Def(Body, "");
  Body += Shader::Replace(oss.str(), "#[Ind]",
                          "_" + std::to_string(Shader::Indexer) + "_");

  return *this;
}

ShaderBuilder& ShaderBuilder::Multi(const std::vector<Material*>& mats,
                                    bool combine)
{
  ++Shader::Indexer;
  std::stringstream pre;
  std::array<std::string, 4> ps{{"", "", "", ""}};
  std::stringstream psh;
  psh << "0.0";

  for (size_t i = 0; i < mats.size(); ++i) {
    pre << " vec4 result#[Ind]" << i << ";result#[Ind]" << i
        << " = vec4(0.,0.,0.,0.); float rp#[Ind]" << i
        << " = " /*<< Shader::Print(mats[i]->opacity)*/ << "; \n";
    pre /*<< mats[i].result*/ << "\n";
    pre << " result#[Ind]" << i << " = result; \n";

    const std::string iStr = std::to_string(i);
    const std::string comp = (i == 0 ? "" : " + ");
    ps[0] += comp + "result#[Ind]" + iStr + ".x*rp#[Ind]" + iStr;
    ps[1] += comp + "result#[Ind]" + iStr + ".y*rp#[Ind]" + iStr;
    ps[2] += comp + "result#[Ind]" + iStr + ".z*rp#[Ind]" + iStr;
    ps[3] += comp + "result#[Ind]" + iStr + ".w*rp#[Ind]" + iStr;

    psh << "+" /*<< Shader::Print(mats[i]->opacity)*/;
  }

  if (combine) {
    const std::string pshStr = Shader::Print(psh.str());

    ps[0] = "(" + ps[0] + ")/(" + pshStr + ")";
    ps[1] = "(" + ps[1] + ")/(" + pshStr + ")";
    ps[2] = "(" + ps[2] + ")/(" + pshStr + ")";
    ps[3] = "(" + ps[3] + ")/(" + pshStr + ")";
  }

  pre << "result = vec4(" << ps[0] << "," << ps[1] << "," << ps[2] << ","
      << ps[3] << ");";

  Body = Shader::Def(Body, "");
  Body += Shader::Replace(pre.str(), "#[Ind]",
                          "_" + std::to_string(Shader::Indexer) + "_");

  return *this;
}

ShaderBuilder& ShaderBuilder::Back(const std::string& mat)
{
  Shader::Me->Setting.Back = true;

  std::ostringstream oss;
  oss << Shader::Def(Body, "") << "if("
      << ShaderMaterialHelperStatics::face_back << "){" << mat << ";}";

  Body = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::InLine(const std::string& mat)
{
  Body = Shader::Def(Body, "") + mat;

  return *this;
}

ShaderBuilder& ShaderBuilder::Front(const std::string& mat)
{
  std::ostringstream oss;
  oss << Shader::Def(Body, "");
  oss << "if(" << ShaderMaterialHelperStatics::face_front << "){" << mat
      << ";}";

  Body = oss.str();

  return *this;
}

ShaderBuilder& ShaderBuilder::Range(const std::string& mat1,
                                    const std::string& mat2,
                                    const IRange& option)
{
  ++Shader::Indexer;
  auto k = std::to_string(Shader::Indexer);

  std::ostringstream oss;
  oss << "float s_r_dim#[Ind] = " << option.direction << ";"
      << "\n";
  oss << "if(s_r_dim#[Ind] > " << Shader::Print(option.end) + "){"
      << "\n";
  oss << mat2 << "\n";
  oss << "}"
      << "\n";
  oss << "else { "
      << "\n";
  oss << mat1 << "\n";
  oss << "   vec4 mat1#[Ind]; mat1#[Ind]  = result;"
      << "\n";
  oss << "   if(s_r_dim#[Ind] > " + Shader::Print(option.start) + "){ "
      << "\n";
  oss << mat2 << "\n";
  oss << " vec4 mati2#[Ind];mati2#[Ind] = result;"
      << "\n";
  oss << " float s_r_cp#[Ind]  = (s_r_dim#[Ind] - ("
      << Shader::Print(option.start) << "))/(" << Shader::Print(option.end)
      << "-(" + Shader::Print(option.start) + "));"
      << "\n";
  oss << " float s_r_c#[Ind]  = 1.0 - s_r_cp#[Ind];"
      << "\n";
  oss << " result = "
      << "vec4(mat1#[Ind].x*s_r_c#[Ind]+mati2#[Ind].x*s_r_cp#[Ind],mat1#[Ind]."
      << "y*s_r_c#[Ind]+mati2#[Ind].y*s_r_cp#[Ind],mat1#[Ind].z*s_r_c#[Ind]+"
      << "mati2#[Ind].z*s_r_cp#[Ind],mat1#[Ind].w*s_r_c#[Ind]+mati2#[Ind].w*s_"
      << "r_cp#[Ind]);"
      << "\n";
  oss << "   }"
      << "\n";
  oss << "   else { result = mat1#[Ind]; }"
      << "\n";
  oss << "}"
      << "\n";
  oss << "result = resHelp#[Ind] ;";

  Body = Shader::Def(Body, "");
  Body += Shader::Replace(oss.str(), "#[Ind]", "_" + k + "_");

  return *this;
}

ShaderBuilder& ShaderBuilder::Reference(size_t index, const std::string& mat)
{
  if (Shader::Me->References.empty()) {
    Shader::Me->References = "";
  }

  std::ostringstream oss;
  oss << "vec4 resHelp#[Ind] = result;";

  std::string indexStr = std::to_string(index);
  if (!String::contains(Shader::Me->References, "," + indexStr + ",")) {
    Shader::Me->References += "," + indexStr + ",";
    oss << " vec4 result_" << index << " = vec4(0.);\n";
  }
  if (mat.empty()) {
    oss << "  result_" << indexStr << " = result;";
  }
  else {
    oss << mat << "\nresult_" << indexStr << " = result;";
  }

  oss << "result = resHelp#[Ind] ;";
  std::string sresult = Shader::Replace(
    oss.str(), "#[Ind]", "_" + std::to_string(Shader::Indexer) + "_");

  Body = Shader::Def(Body, "");
  Body += sresult;

  return *this;
}

ShaderBuilder& ShaderBuilder::ReplaceColor(int index, int color,
                                           const std::string& mat,
                                           const IReplaceColor& option)
{
  ++Shader::Indexer;
  auto d   = option.rangeStep;
  auto d2  = option.rangePower;
  auto d3  = option.colorIndex;
  auto d4  = option.colorStep;
  auto ilg = option.indexToEnd;

  auto indexStr = std::to_string(index);

  auto lg = " > 0.5 + " + Shader::Print(d) + " ";
  auto lw = " < 0.5 - " + Shader::Print(d) + " ";
  auto rr = "((result_" + indexStr + ".x*" + Shader::Print(d4) + "-"
            + Shader::Print(d3) + ")>1.0 ? 0. : max(0.,(result_" + indexStr
            + ".x*" + Shader::Print(d4) + "-" + Shader::Print(d3) + ")))";
  auto rg = "((result_" + indexStr + ".y*" + Shader::Print(d4) + "-"
            + Shader::Print(d3) + ")>1.0 ? 0. : max(0.,(result_" + indexStr
            + ".y*" + Shader::Print(d4) + "-" + Shader::Print(d3) + ")))";
  auto rb = "((result_" + indexStr + ".z*" + Shader::Print(d4) + "-"
            + Shader::Print(d3) + ")>1.0 ? 0. : max(0.,(result_" + indexStr
            + ".z*" + Shader::Print(d4) + "-" + Shader::Print(d3) + ")))";
  if (ilg) {
    rr = "min(1.0, max(0.,(result_" + indexStr + ".x*" + Shader::Print(d4) + "-"
         + Shader::Print(d3) + ")))";
    rg = "min(1.0, max(0.,(result_" + indexStr + ".y*" + Shader::Print(d4) + "-"
         + Shader::Print(d3) + ")))";
    rb = "min(1.0, max(0.,(result_" + indexStr + ".z*" + Shader::Print(d4) + "-"
         + Shader::Print(d3) + ")))";
  }
  auto a = " && ";
  auto p = " + ";

  std::string r    = "";
  std::string cond = "";

  switch (color) {
    case Helper::White:
      cond = rr + lg + a + rg + lg + a + rb + lg;
      r    = "(" + rr + p + rg + p + rb + ")/3.0";
      break;
    case Helper::Cyan:
      cond = rr + lw + a + rg + lg + a + rb + lg;
      r    = "(" + rg + p + rb + ")/2.0 - (" + rr + ")/1.0";
      break;
    case Helper::Pink:
      cond = rr + lg + a + rg + lw + a + rb + lg;
      r    = "(" + rr + p + rb + ")/2.0 - (" + rg + ")/1.0";
      break;
    case Helper::Yellow:
      cond = rr + lg + a + rg + lg + a + rb + lw;
      r    = "(" + rr + p + rg + ")/2.0 - (" + rb + ")/1.0";
      break;
    case Helper::Blue:
      cond = rr + lw + a + rg + lw + a + rb + lg;
      r    = "(" + rb + ")/1.0 - (" + rr + p + rg + ")/2.0";
      break;
    case Helper::Red:
      cond = rr + lg + a + rg + lw + a + rb + lw;
      r    = "(" + rr + ")/1.0 - (" + rg + p + rb + ")/2.0";
      break;
    case Helper::Green:
      cond = rr + lw + a + rg + lg + a + rb + lw;
      r    = "(" + rg + ")/1.0 - (" + rr + p + rb + ")/2.0";
      break;
    case Helper::Black:
      cond = rr + lw + a + rg + lw + a + rb + lw;
      r    = "1.0-(" + rr + p + rg + p + rb + ")/3.0";
      break;
  }

  auto sresult
    = " if( " + cond
      + " ) { vec4 oldrs#[Ind] = vec4(result);float al#[Ind] = max(0.0,min(1.0,"
      + r + "+(" + Shader::Print(d2) + "))); float  l#[Ind] =  1.0-al#[Ind];  "
      + mat + " result = result*al#[Ind] +  oldrs#[Ind] * l#[Ind]; }";

  sresult = Shader::Replace(sresult, "#[Ind]",
                            "_" + std::to_string(Shader::Indexer) + "_");

  Body = Shader::Def(Body, "");
  Body += sresult;

  return *this;
}

ShaderBuilder& ShaderBuilder::Blue(int index, const std::string& mat,
                                   const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Blue, mat, option);
}

ShaderBuilder& ShaderBuilder::Cyan(int index, const std::string& mat,
                                   const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Cyan, mat, option);
}

ShaderBuilder& ShaderBuilder::Red(int index, const std::string& mat,
                                  const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Red, mat, option);
}

ShaderBuilder& ShaderBuilder::Yellow(int index, const std::string& mat,
                                     const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Yellow, mat, option);
}

ShaderBuilder& ShaderBuilder::Green(int index, const std::string& mat,
                                    const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Green, mat, option);
}

ShaderBuilder& ShaderBuilder::Pink(int index, const std::string& mat,
                                   const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Pink, mat, option);
}

ShaderBuilder& ShaderBuilder::White(int index, const std::string& mat,
                                    const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::White, mat, option);
}

ShaderBuilder& ShaderBuilder::Black(int index, const std::string& mat,
                                    const IReplaceColor& option)
{
  return ReplaceColor(index, Helper::Black, mat, option);
}

ShaderBuilder& ShaderBuilder::ReflectCube(IReflectMap& option)
{
  Shader::Indexer++;
  int s = Shader::Me->GetCubeMapIndex(option.path);

  if (s == -1) {
    Shader::Me->Setting.TextureCubes.emplace_back(
      ITexture(option.path, option.useInVertex, true));
  }
  else {
    size_t index                                     = static_cast<size_t>(s);
    Shader::Me->Setting.TextureCubes[index].inVertex = true;
  }

  s = Shader::Me->GetCubeMapIndex(option.path);

  std::ostringstream oss;

  if (option.equirectangular) {
    option.path = Shader::Def(option.path, "/images/cube/roofl1.jpg");
    s           = Shader::Me->GetMapIndex(option.path);

    if (s == -1) {
      Shader::Me->Setting.Texture2Ds.emplace_back(
        ITexture(option.path, option.useInVertex, true));
    }
    else {
      size_t index                                   = static_cast<size_t>(s);
      Shader::Me->Setting.Texture2Ds[index].inVertex = true;
    }

    s                                 = Shader::Me->GetMapIndex(option.path);
    Shader::Me->Setting.VertexWorld   = true;
    Shader::Me->Setting.FragmentWorld = true;

    oss << " vec3 nWorld#[Ind] = normalize( mat3( world[0].xyz, world[1].xyz, "
           "world[2].xyz ) *  "
        << option.normal << "); "
        << " vec3 vReflect#[Ind] = normalize( reflect( normalize(  "
        << ShaderMaterialHelperStatics::Camera << "- vec3(world * vec4("
        << ShaderMaterialHelperStatics::Position
        << ", 1.0))),  nWorld#[Ind] ) ); "
        << "float yaw#[Ind] = .5 - atan( vReflect#[Ind].z, -1.* "
           "vReflect#[Ind].x ) / ( 2.0 * 3.14159265358979323846264);  "
        << " float pitch#[Ind] = .5 - atan( vReflect#[Ind].y, length( "
           "vReflect#[Ind].xz ) ) / ( 3.14159265358979323846264);  "
        << " vec3 color#[Ind] = texture2D( ' + "
           "ShaderMaterialHelperStatics::Texture2D + s + ', vec2( yaw#[Ind], "
           "pitch#[Ind])"
        << (option.bias.empty() || Shader::Print(option.bias) == "0." ?
              "" :
              "," + Shader::Print(option.bias))
             + " ).rgb; result = vec4(color#[Ind] ,1.);";
  }
  else {
    option.path = Shader::Def(option.path, "/images/cube/a");

    oss << "vec3 viewDir#[Ind] =  " << ShaderMaterialHelperStatics::Position
        << " - " << ShaderMaterialHelperStatics::Camera << " ;";
    oss << "\n";
    oss << "  viewDir#[Ind] =r_x(viewDir#[Ind] ,"
        << Shader::Print(option.rotation.x) << ",  "
        << ShaderMaterialHelperStatics::Center << ");";
    oss << "\n";
    oss << "  viewDir#[Ind] =r_y(viewDir#[Ind] ,"
        << Shader::Print(option.rotation.y) << ","
        << ShaderMaterialHelperStatics::Center << ");";
    oss << "\n";
    oss << "  viewDir#[Ind] =r_z(viewDir#[Ind] ,"
        << Shader::Print(option.rotation.z)
        << "," + std::string(ShaderMaterialHelperStatics::Center) << ");";
    oss << "\n";
    oss << "vec3 coords#[Ind] = " << (option.refract ? "refract" : "reflect")
        << "(viewDir#[Ind]" << (option.revers ? "*vec3(1.0)" : "*vec3(-1.0)")
        << ", " + option.normal << " "
        << (option.refract ? ",(" + Shader::Print(option.refractMap) + ")" : "")
        << " )+" + std::string(ShaderMaterialHelperStatics::Position) + "; ";
    oss << "\n";
    oss << "vec3 vReflectionUVW#[Ind] = vec3( "
        << std::string(ShaderMaterialHelperStatics::ReflectMatrix)
        << " *  vec4(coords#[Ind], 0)); ";
    oss << "\n";
    oss << "vec3 rc#[Ind]= textureCube("
        << std::string(ShaderMaterialHelperStatics::TextureCube) << s
        << ", vReflectionUVW#[Ind] "
             + (option.bias.empty() || Shader::Print(option.bias) == "0." ?
                  "" :
                  "," + Shader::Print(option.bias))
             + ").rgb;";
    oss << "\n";
    oss << "result =result  + vec4(rc#[Ind].x ,rc#[Ind].y,rc#[Ind].z, "
        << (!option.alpha ? "1." : "(rc#[Ind].x+rc#[Ind].y+rc#[Ind].z)/3.0 ")
        << ")*(min(1.,max(0.," + Shader::Print(option.reflectMap) << ")));  ";
  }

  return *this;
}

ShaderBuilder& ShaderBuilder::NormalMap(const std::string& val,
                                        const std::string& mat)
{
  Shader::Me->Setting.NormalOpacity = val;
  Shader::Me->Setting.NormalMap     = mat;

  return *this;
}

ShaderBuilder& ShaderBuilder::SpecularMap(const std::string& mat)
{
  Shader::Me->Setting.SpecularMap = mat;

  return *this;
}

std::unique_ptr<ShaderBuilder> ShaderBuilder::Instance()
{
  auto setting      = Shader::Me->Setting;
  auto instance     = std::make_unique<ShaderBuilder>();
  instance->Parent  = Shader::Me;
  instance->Setting = setting;

  return instance;
}

ShaderBuilder& ShaderBuilder::Reflect(const IReflectMap& /*option*/,
                                      float /*opacity*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Light(const ILight& option)
{
  IColor c_c;
  if (option.darkColorMode) {
    c_c.a = 1.f - c_c.a;

    c_c.r = 1.f - c_c.r;
    c_c.g = 1.f - c_c.g;
    c_c.b = 1.f - c_c.b;
    c_c.a = c_c.a - 1.f;
  }

  ++Shader::Indexer;

  Shader::Me->Setting.Camera        = true;
  Shader::Me->Setting.FragmentWorld = true;
  Shader::Me->Setting.VertexWorld   = true;
  Shader::Me->Setting.Helpers       = true;
  Shader::Me->Setting.Center        = true;

  std::ostringstream oss;
  oss << "  vec3 dir#[Ind] = normalize(  vec3(world * vec4("
      << ShaderMaterialHelperStatics::Position << ",1.)) - "
      << ShaderMaterialHelperStatics::Camera << ");"
      << "\n"
      << "  dir#[Ind] =r_x(dir#[Ind] ," << Shader::Print(option.rotation.x)
      << ",vec3(" << ShaderMaterialHelperStatics::Center << "));"
      << "\n"
      << "  dir#[Ind] =r_y(dir#[Ind] ," << Shader::Print(option.rotation.y)
      << ",vec3(" << ShaderMaterialHelperStatics::Center << "));"
      << "\n"
      << "  dir#[Ind] =r_z(dir#[Ind] ," << Shader::Print(option.rotation.z)
      << ",vec3(" << ShaderMaterialHelperStatics::Center << "));";
  oss << "\n";
  oss << "  vec3 vnrm#[Ind] = normalize(vec3(world * vec4(" << option.normal
      << ", 0.0)));"
      << "\n";
  oss << "\n";
  oss << "  vec3 l#[Ind]= normalize(p1#[Ind].xyz "
      << (!option.parallel ?
            "- vec3(world * vec4("
              + std::string(ShaderMaterialHelperStatics::Position) + ",1.))  " :
            "")
      << ");"
      << "\n";
  oss << "\n";
  oss << "  vec3 vw#[Ind]= normalize(camera -  vec3(world * vec4("
      << ShaderMaterialHelperStatics::Position << ",1.)));"
      << "\n";
  oss << "\n";
  oss << "vec3 aw#[Ind]= normalize(vw#[Ind]+ l#[Ind]);"
      << "\n";
  oss << "\n";
  oss << "float sc#[Ind]= max(0.,min(1., dot(vnrm#[Ind], aw#[Ind])));"
      << "\n";
  oss << "\n";
  oss << "sc#[Ind]= pow(sc#[Ind]*min(1.,max(0.,"
      << Shader::Print(option.specular) + ")), ("
      << Shader::Print(option.specularPower * 1000.f) << "))/"
      << Shader::Print(option.specularLevel) << ";"
      << "\n";
  oss << "\n";
  oss << " float  ph#[Ind]= pow(" + Shader::Print(option.phonge) + "*2., ("
           + Shader::Print(option.phongePower) + "*0.3333))/("
           + Shader::Print(option.phongeLevel) + "*3.);";
  oss << "\n";
  oss << "  float ndl#[Ind] = max(0., dot(vnrm#[Ind], l#[Ind]));";
  oss << "\n";
  oss << "  float ls#[Ind] = " << (option.supplement ? "1.0 -" : "")
      << "max(0.,min(1.,ndl#[Ind]*ph#[Ind]*("
      << Shader::Print(option.reducer) + ")));";
  oss << "\n";
  oss << "  result  += vec4( c1#[Ind].xyz*( ls#[Ind])*" + Shader::Print(c_c.a)
           + " ,  ls#[Ind]); ";
  oss << "\n";
  oss << "  float ls2#[Ind] = " << (option.supplement ? "0.*" : "1.*")
      << "max(0.,min(1., sc#[Ind]*(" << Shader::Print(option.reducer) + ")));";
  oss << "\n";
  oss << "  result  += vec4( c1#[Ind].xyz*( ls2#[Ind])*" << Shader::Print(c_c.a)
      << " ,  ls2#[Ind]); ";

  std::string sresult = Shader::Replace(
    oss.str(), "#[Ind]", "_" + std::to_string(Shader::Indexer) + "_");

  Body = Shader::Def(Body, "");
  Body += sresult;

  return *this;
}

ShaderBuilder& ShaderBuilder::Effect(const IEffect& option)
{
  const IEffect& op = option;
  ++Shader::Indexer;

  std::ostringstream oss;
  oss << "vec4 res#[Ind] = vec4(0.);\n";
  oss << "res#[Ind].x = "
      << (!op.px.empty() ?
            Shader::Replace(
              Shader::Replace(
                Shader::Replace(Shader::Replace(op.px, "px", "result.x"), "py",
                                "result.y"),
                "pz", "result.z"),
              "pw", "result.w")
              + ";" :
            " result.x;");
  oss << "\n";
  oss << "res#[Ind].y = "
      << (!op.py.empty() ?
            Shader::Replace(
              Shader::Replace(
                Shader::Replace(Shader::Replace(op.py, "px", "result.x"), "py",
                                "result.y"),
                "pz", "result.z"),
              "pw", "result.w")
              + ";" :
            " result.y;");
  oss << "\n";
  oss << "res#[Ind].z = "
      << (!op.pz.empty() ?
            Shader::Replace(
              Shader::Replace(
                Shader::Replace(Shader::Replace(op.pz, "px", "result.x"), "py",
                                "result.y"),
                "pz", "result.z"),
              "pw", "result.w")
              + ";" :
            " result.z;");
  oss << "\n";
  oss << "res#[Ind].w = "
      << (!op.pw.empty() ?
            Shader::Replace(
              Shader::Replace(
                Shader::Replace(Shader::Replace(op.pw, "px", "result.x"), "py",
                                "result.y"),
                "pz", "result.z"),
              "pw", "result.w")
              + ";" :
            " result.w;");
  oss
    << "res#[Ind]  = "
    << (!op.pr.empty() ?
          " vec4("
            + Shader::Replace(
                Shader::Replace(
                  Shader::Replace(
                    Shader::Replace(Shader::Replace(op.pr, "pr", "res#[Ind].x"),
                                    "px", "result.x"),
                    "py", "result.y"),
                  "pz", "result.z"),
                "pw", "result.w")
            + ","
            + Shader::Replace(
                Shader::Replace(
                  Shader::Replace(
                    Shader::Replace(Shader::Replace(op.pr, "pr", "res#[Ind].y"),
                                    "px", "result.x"),
                    "py", "result.y"),
                  "pz", "result.z"),
                "pw", "result.w")
            + ","
            + Shader::Replace(
                Shader::Replace(
                  Shader::Replace(
                    Shader::Replace(Shader::Replace(op.pr, "pr", "res#[Ind].z"),
                                    "px", "result.x"),
                    "py", "result.y"),
                  "pz", "result.z"),
                "pw", "result.w")
            + ","
            + Shader::Replace(
                Shader::Replace(
                  Shader::Replace(
                    Shader::Replace(Shader::Replace(op.pr, "pr", "res#[Ind].w"),
                                    "px", "result.x"),
                    "py", "result.y"),
                  "pz", "result.z"),
                "pw", "result.w")
            + ");" :
          " res#[Ind]*1.0;");
  oss << "\n";
  oss << "result = res#[Ind] ;";

  std::string sresult = Shader::Replace(
    oss.str(), "#[Ind]", "_" + std::to_string(Shader::Indexer) + "_");

  Body = Shader::Def(Body, "");
  Body += sresult;

  return *this;
}

ShaderBuilder& ShaderBuilder::IdColor(float /*id*/, float /*w*/)
{
  return *this;
}

ShaderBuilder& ShaderBuilder::Discard()
{
  Body = Shader::Def(Body, "");
  Body += "discard;";

  return *this;
}

} // end of namespace Extensions
} // end of namespace BABYLON
