#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_BUILDER_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_BUILDER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/extensions/shaderbuilder/shader_setting.h>

namespace BABYLON {

class Camera;
class Material;
class PostProcess;
class Scene;
class ShaderMaterial;

namespace Extensions {

struct IColor;
struct IEffect;
struct ILight;
struct IMap;
struct INut;
struct IRange;
struct IReplaceColor;
struct IReflectMap;

class BABYLON_SHARED_EXPORT ShaderBuilder {

public:
  ShaderBuilder();
  ~ShaderBuilder();

  void PrepareBeforeMaterialBuild();
  void PrepareBeforePostProcessBuild();
  ShaderMaterial* PrepareMaterial(ShaderMaterial* material, Scene* scene);
  std::string Build();
  std::string BuildVertex();
  ShaderBuilder& SetUniform(const std::string& name, const std::string& type);
  ShaderMaterial* BuildMaterial(Scene* scene);
  PostProcess* BuildPostProcess(Camera* camera, Scene* scene, float scale,
                                const std::string& option);
  ShaderBuilder& Event(int index, const std::string& mat);
  ShaderBuilder& EventVertex(int index, const std::string& mat);
  ShaderBuilder& Transparency();
  ShaderBuilder& PostEffect1(size_t id, const std::string& effect);
  ShaderBuilder& PostEffect2(size_t id, const std::string& effect);
  ShaderBuilder& ImportSamplers(const std::vector<std::string>& txts);
  ShaderBuilder& Wired();
  ShaderBuilder& VertexShader(const std::string& mat);
  ShaderBuilder& Solid(const IColor& color);
  int GetMapIndex(const std::string key);
  int GetCubeMapIndex(const std::string key);
  // Func
  std::string Nut(std::string& value, const INut& option);
  ShaderBuilder& Map(IMap& option);
  ShaderBuilder& Multi(const std::vector<Material*>& mats, bool combine = true);
  ShaderBuilder& Back(const std::string& mat = "");
  ShaderBuilder& InLine(const std::string& mat = "");
  ShaderBuilder& Front(const std::string& mat = "");
  ShaderBuilder& Range(const std::string& mat1, const std::string& mat2,
                       const IRange& option);
  ShaderBuilder& Reference(size_t index, const std::string& mat);
  ShaderBuilder& ReplaceColor(int index, int color, const std::string& mat,
                              const IReplaceColor& option);
  ShaderBuilder& Blue(int index, const std::string& mat,
                      const IReplaceColor& option);
  ShaderBuilder& Cyan(int index, const std::string& mat,
                      const IReplaceColor& option);
  ShaderBuilder& Red(int index, const std::string& mat,
                     const IReplaceColor& option);
  ShaderBuilder& Yellow(int index, const std::string& mat,
                        const IReplaceColor& option);
  ShaderBuilder& Green(int index, const std::string& mat,
                       const IReplaceColor& option);
  ShaderBuilder& Pink(int index, const std::string& mat,
                      const IReplaceColor& option);
  ShaderBuilder& White(int index, const std::string& mat,
                       const IReplaceColor& option);
  ShaderBuilder& Black(int index, const std::string& mat,
                       const IReplaceColor& option);
  ShaderBuilder& ReflectCube(IReflectMap& option);
  ShaderBuilder& NormalMap(const std::string& val, const std::string& mat);
  ShaderBuilder& SpecularMap(const std::string& mat);
  std::unique_ptr<ShaderBuilder> Instance();
  ShaderBuilder& Reflect(const IReflectMap& option, float opacity = 1.f);
  ShaderBuilder& Light(const ILight& option);
  ShaderBuilder& Effect(const IEffect& option);
  ShaderBuilder& IdColor(float id, float w);
  ShaderBuilder& Discard();

public:
  ShaderBuilder* Parent;
  ShaderSetting Setting;
  int CustomIndexer;
  std::vector<std::string> Fragment;
  std::vector<std::string> Vertex;

  std::string FragmentBeforeMain;
  std::string VertexBeforeMain;
  std::vector<std::string> Varings;
  std::vector<std::string> Attributes;
  std::vector<std::string> Uniforms;
  std::string FragmentUniforms;
  std::string VertexUniforms;
  std::vector<std::string> Extentions;
  std::string References;
  std::vector<std::string> Helpers;
  std::string Body;
  std::string VertexBody;
  std::string AfterVertex;
  std::string RenderTargetForColorId;
  std::vector<std::string> PPSSamplers;
  std::string RenderTargetForDepth;

  std::vector<std::string> PostEffect1Effects;
  std::vector<std::string> PostEffect2Effects;

}; // end of struct ShaderBuilder

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_BUILDER_H
