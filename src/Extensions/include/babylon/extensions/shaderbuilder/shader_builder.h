#ifndef BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_BUILDER_H
#define BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_BUILDER_H

#include <babylon/babylon_global.h>
#include <babylon/extensions/shaderbuilder/shader_setting.h>

namespace BABYLON {
namespace Extensions {

struct BABYLON_SHARED_EXPORT ShaderBuilder {

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

  void PrepareBeforeMaterialBuild();
  void PrepareBeforePostProcessBuild();
  // ShaderMaterial* PrepareMaterial(ShaderMaterial* material, Scene* scene);
  // std::string Build();
  // std::string BuildVertex();
  // ShaderBuilder& SetUniform(const std::string& name, const std::string&
  // type);
  // ShaderMaterial* BuildMaterial(Scene* scene);
  // PostProcess* BuildPostProcess(Camera* camera, Scene* scene, float scale,
  //                              const std::string& option);

}; // end of struct ShaderBuilder

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_SHADER_BUILDER_SHADER_BUILDER_H
