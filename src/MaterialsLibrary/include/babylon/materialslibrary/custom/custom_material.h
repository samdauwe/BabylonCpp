#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materialslibrary/custom/shader_special_parts.h>
#include <babylon/maths/vector4.h>
#include <unordered_map>

namespace BABYLON {

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

namespace MaterialsLibrary {

struct UniformInstance {
  float floatValue;
  Vector2 vec2;
  Vector3 vec3;
  Vector4 vec4;
  Matrix mat4;
  Texture* sampler2D;
}; // end of struct UniformInstance

class BABYLON_SHARED_EXPORT CustomMaterial : public StandardMaterial {

public:
  CustomMaterial(const std::string& name, Scene* scene);
  ~CustomMaterial() override; // = default

  void AttachAfterBind(Mesh* mesh, Effect* effect);
  std::vector<std::string> ReviewUniform(const std::string& name, std::vector<std::string> arr);
  std::string Builder(const std::string& shaderName, const std::vector<std::string>& uniforms,
                      const std::vector<std::string>& uniformBuffers,
                      const std::vector<std::string>& samplers, StandardMaterialDefines& defines,
                      std::vector<std::string> attributes = {});
  CustomMaterial& AddUniform(const std::string& name, const std::string& kind,
                             const std::optional<std::variant<UniformInstance, TexturePtr>>& param);
  CustomMaterial& AddAttribute(const std::string& name);
  CustomMaterial& Fragment_Begin(const std::string& shaderPart);
  CustomMaterial& Fragment_Definitions(const std::string& shaderPart);
  CustomMaterial& Fragment_MainBegin(const std::string& shaderPart);
  CustomMaterial& Fragment_Custom_Diffuse(const std::string& shaderPart);
  CustomMaterial& Fragment_Custom_Alpha(const std::string& shaderPart);
  CustomMaterial& Fragment_Before_Lights(const std::string& shaderPart);
  CustomMaterial& Fragment_Before_Fog(const std::string& shaderPart);
  CustomMaterial& Fragment_Before_FragColor(const std::string& shaderPart);
  CustomMaterial& Vertex_Begin(const std::string& shaderPart);
  CustomMaterial& Vertex_Definitions(const std::string& shaderPart);
  CustomMaterial& Vertex_MainBegin(const std::string& shaderPart);
  CustomMaterial& Vertex_Before_PositionUpdated(const std::string& shaderPart);
  CustomMaterial& Vertex_Before_NormalUpdated(const std::string& shaderPart);
  CustomMaterial& Vertex_After_WorldPosComputed(const std::string& shaderPart);
  CustomMaterial& Vertex_MainEnd(const std::string& shaderPart);

private:
  void _afterBind(Mesh* mesh, Effect* effect);

public:
  static unsigned int ShaderIndexer;
  ShaderSpecialParts CustomParts;
  bool _isCreatedShader;
  std::string _createdShaderName;
  std::vector<std::string> _customUniform;
  std::vector<std::string> _newUniforms;
  std::unordered_map<std::string, UniformInstance> _newUniformInstances;
  std::unordered_map<std::string, TexturePtr> _newSamplerInstances;
  std::vector<std::string> _customAttributes;

  std::string FragmentShader;
  std::string VertexShader;

}; // end of class CustomMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H
