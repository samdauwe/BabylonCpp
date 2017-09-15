#ifndef BABYLON_MATERIALS_SHADER_MATERIAL_H
#define BABYLON_MATERIALS_SHADER_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/material.h>
#include <babylon/materials/shader_material_options.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ShaderMaterial : public Material {

public:
  template <typename... Ts>
  static ShaderMaterial* New(Ts&&... args)
  {
    auto material = new ShaderMaterial(::std::forward<Ts>(args)...);
    material->addMaterialToScene(static_cast<unique_ptr_t<Material>>(material));

    return material;
  }
  virtual ~ShaderMaterial();

  /**
   * @brief Returns the string "ShaderMaterial".
   */
  const char* getClassName() const;

  IReflect::Type type() const override;

  bool needAlphaBlending() override;
  bool needAlphaTesting() override;
  ShaderMaterial& setTexture(const string_t& name, Texture* texture);
  ShaderMaterial& setTextureArray(const string_t& iName,
                                  const vector_t<BaseTexture*>& textures);
  ShaderMaterial& setFloat(const string_t& name, float value);
  ShaderMaterial& setFloats(const string_t& name, const Float32Array& value);
  ShaderMaterial& setColor3(const string_t& name, const Color3& value);
  ShaderMaterial& setColor3Array(const string_t& iName,
                                 const vector_t<Color3>& value);
  ShaderMaterial& setColor4(const string_t& name, const Color4& value);
  ShaderMaterial& setVector2(const string_t& name, const Vector2& value);
  ShaderMaterial& setVector3(const string_t& name, const Vector3& value);
  ShaderMaterial& setVector4(const string_t& name, const Vector4& value);
  ShaderMaterial& setMatrix(const string_t& name, const Matrix& value);
  ShaderMaterial& setMatrix3x3(const string_t& name, const Float32Array& value);
  ShaderMaterial& setMatrix2x2(const string_t& name, const Float32Array& value);
  ShaderMaterial& setArray3(const string_t& iName, const Float32Array& value);
  bool isReady(AbstractMesh* mesh = nullptr,
               bool useInstances  = false) override;
  void bindOnlyWorldMatrix(Matrix& world) override;
  void bind(Matrix* world, Mesh* mesh = nullptr) override;
  vector_t<BaseTexture*> getActiveTextures() const override;
  bool hasTexture(BaseTexture* texture) const override;
  Material* clone(const string_t& name,
                  bool cloneChildren = false) const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Json::object serialize() const;

  // Statics
  static unique_ptr_t<ShaderMaterial> Parse(const Json::value& source,
                                            Scene* scene, const string_t& url);

protected:
  ShaderMaterial(const string_t& name, Scene* scene, const string_t& shaderPath,
                 const ShaderMaterialOptions& options);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);
  void _checkUniform(const string_t& uniformName);

private:
  string_t _shaderPath;
  ShaderMaterialOptions _options;
  unordered_map_t<string_t, Texture*> _textures;
  unordered_map_t<string_t, vector_t<BaseTexture*>> _textureArrays;
  unordered_map_t<string_t, float> _floats;
  unordered_map_t<string_t, Float32Array> _floatsArrays;
  unordered_map_t<string_t, Color3> _colors3;
  unordered_map_t<string_t, Float32Array> _colors3Arrays;
  unordered_map_t<string_t, Color4> _colors4;
  unordered_map_t<string_t, Vector2> _vectors2;
  unordered_map_t<string_t, Vector3> _vectors3;
  unordered_map_t<string_t, Vector4> _vectors4;
  unordered_map_t<string_t, Matrix> _matrices;
  unordered_map_t<string_t, Float32Array> _matrices3x3;
  unordered_map_t<string_t, Float32Array> _matrices2x2;
  unordered_map_t<string_t, Float32Array> _vectors3Arrays;
  Matrix _cachedWorldViewMatrix;
  int _renderId;

}; // end of class ShaderMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_SHADER_MATERIAL_H
