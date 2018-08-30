#ifndef BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materialslibrary/custom/shader_special_parts.h>
#include <babylon/math/vector4.h>

namespace BABYLON {
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
  CustomMaterial(const string_t& name, Scene* scene);
  ~CustomMaterial();

  void AttachAfterBind(Mesh* mesh, Effect* effect);
  vector_t<string_t> ReviewUniform(const string_t& name,
                                   vector_t<string_t> arr);
  string_t Builder(const string_t& shaderName,
                   const vector_t<string_t>& uniforms,
                   const vector_t<string_t>& uniformBuffers,
                   const vector_t<string_t>& samplers,
                   StandardMaterialDefines& defines);
  CustomMaterial& AddUniform(const string_t& name, const string_t& kind,
                             const nullable_t<UniformInstance>& param);
  CustomMaterial& Fragment_Begin(const string_t& shaderPart);
  CustomMaterial& Fragment_Definitions(const string_t& shaderPart);
  CustomMaterial& Fragment_MainBegin(const string_t& shaderPart);
  CustomMaterial& Fragment_Custom_Diffuse(const string_t& shaderPart);
  CustomMaterial& Fragment_Custom_Alpha(const string_t& shaderPart);
  CustomMaterial& Fragment_Before_FragColor(const string_t& shaderPart);
  CustomMaterial& Vertex_Begin(const string_t& shaderPart);
  CustomMaterial& Vertex_Definitions(const string_t& shaderPart);
  CustomMaterial& Vertex_MainBegin(const string_t& shaderPart);
  CustomMaterial& Vertex_Before_PositionUpdated(const string_t& shaderPart);
  CustomMaterial& Vertex_Before_NormalUpdated(const string_t& shaderPart);

private:
  void _afterBind(Mesh* mesh, Effect* effect);

public:
  static unsigned int ShaderIndexer;
  ShaderSpecialParts CustomParts;
  bool _isCreatedShader;
  string_t _createdShaderName;
  vector_t<string_t> _customUniform;
  vector_t<string_t> _newUniforms;
  unordered_map_t<string_t, UniformInstance> _newUniformInstances;
  unordered_map_t<string_t, TexturePtr> _newSamplerInstances;

  string_t FragmentShader;
  string_t VertexShader;

}; // end of class CustomMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_CUSTOM_CUSTOM_MATERIAL_H
