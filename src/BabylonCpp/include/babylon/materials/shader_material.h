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
    auto material = new ShaderMaterial(std::forward<Ts>(args)...);
    material->addMaterialToScene(
      static_cast<std::unique_ptr<Material>>(material));

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
  ShaderMaterial& setTexture(const std::string& name, Texture* texture);
  ShaderMaterial& setTextureArray(const std::string& iName,
                                  const std::vector<BaseTexture*>& textures);
  ShaderMaterial& setFloat(const std::string& name, float value);
  ShaderMaterial& setFloats(const std::string& name,
                            const std::vector<float>& value);
  ShaderMaterial& setColor3(const std::string& name, const Color3& value);
  ShaderMaterial& setColor4(const std::string& name, const Color4& value);
  ShaderMaterial& setVector2(const std::string& name, const Vector2& value);
  ShaderMaterial& setVector3(const std::string& name, const Vector3& value);
  ShaderMaterial& setVector4(const std::string& name, const Vector4& value);
  ShaderMaterial& setMatrix(const std::string& name, const Matrix& value);
  ShaderMaterial& setMatrix3x3(const std::string& name,
                               const Float32Array& value);
  ShaderMaterial& setMatrix2x2(const std::string& name,
                               const Float32Array& value);
  ShaderMaterial& setArray3(const std::string& iName,
                            const Float32Array& value);
  bool isReady(AbstractMesh* mesh = nullptr,
               bool useInstances  = false) override;
  void bindOnlyWorldMatrix(Matrix& world) override;
  void bind(Matrix* world, Mesh* mesh = nullptr) override;
  std::vector<BaseTexture*> getActiveTextures() const override;
  Material* clone(const std::string& name,
                  bool cloneChildren = false) const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  Json::object serialize() const;

  // Statics
  static std::unique_ptr<ShaderMaterial>
  Parse(const Json::value& source, Scene* scene, const std::string& url);

protected:
  ShaderMaterial(const std::string& name, Scene* scene,
                 const std::string& shaderPath,
                 const ShaderMaterialOptions& options);

private:
  bool _checkCache(Scene* scene, AbstractMesh* mesh, bool useInstances = false);
  void _checkUniform(const std::string& uniformName);

private:
  std::string _shaderPath;
  ShaderMaterialOptions _options;
  std::unordered_map<std::string, Texture*> _textures;
  std::unordered_map<std::string, std::vector<BaseTexture*>> _textureArrays;
  std::unordered_map<std::string, float> _floats;
  std::unordered_map<std::string, std::vector<float>> _floatsArrays;
  std::unordered_map<std::string, Color3> _colors3;
  std::unordered_map<std::string, Color4> _colors4;
  std::unordered_map<std::string, Vector2> _vectors2;
  std::unordered_map<std::string, Vector3> _vectors3;
  std::unordered_map<std::string, Vector4> _vectors4;
  std::unordered_map<std::string, Matrix> _matrices;
  std::unordered_map<std::string, Float32Array> _matrices3x3;
  std::unordered_map<std::string, Float32Array> _matrices2x2;
  std::unordered_map<std::string, Float32Array> _vectors3Arrays;
  Matrix _cachedWorldViewMatrix;
  int _renderId;

}; // end of class ShaderMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_SHADER_MATERIAL_H
