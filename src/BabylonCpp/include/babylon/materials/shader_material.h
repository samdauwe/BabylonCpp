#ifndef BABYLON_MATERIALS_SHADER_MATERIAL_H
#define BABYLON_MATERIALS_SHADER_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material.h>
#include <babylon/materials/shader_material_options.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>

namespace BABYLON {

class ShaderMaterial;
class Texture;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;
using TexturePtr        = std::shared_ptr<Texture>;

class BABYLON_SHARED_EXPORT ShaderMaterial : public Material {

public:
  template <typename... Ts>
  static ShaderMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<ShaderMaterial>(
      new ShaderMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  virtual ~ShaderMaterial() override;

  /**
   * @brief Returns the string "ShaderMaterial".
   */
  const std::string getClassName() const override;

  IReflect::Type type() const override;

  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  ShaderMaterial& setTexture(const std::string& name,
                             const TexturePtr& texture);
  ShaderMaterial& setTextureArray(const std::string& iName,
                                  const std::vector<BaseTexturePtr>& textures);
  ShaderMaterial& setInt(const std::string& name, int value);
  ShaderMaterial& setFloat(const std::string& name, float value);
  ShaderMaterial& setFloats(const std::string& name, const Float32Array& value);
  ShaderMaterial& setColor3(const std::string& name, const Color3& value);
  ShaderMaterial& setColor3Array(const std::string& iName,
                                 const std::vector<Color3>& value);
  ShaderMaterial& setColor4(const std::string& name, const Color4& value);
  ShaderMaterial& setVector2(const std::string& name, const Vector2& value);
  ShaderMaterial& setVector3(const std::string& name, const Vector3& value);
  ShaderMaterial& setVector4(const std::string& name, const Vector4& value);
  ShaderMaterial& setMatrix(const std::string& name, const Matrix& value);
  ShaderMaterial& setMatrix3x3(const std::string& name,
                               const Float32Array& value);
  ShaderMaterial& setMatrix2x2(const std::string& name,
                               const Float32Array& value);
  ShaderMaterial& setArray2(const std::string& iName,
                            const Float32Array& value);
  ShaderMaterial& setArray3(const std::string& iName,
                            const Float32Array& value);
  bool isReady(AbstractMesh* mesh = nullptr,
               bool useInstances  = false) override;
  void bindOnlyWorldMatrix(Matrix& world) override;
  void bind(Matrix* world, Mesh* mesh = nullptr) override;
  std::vector<BaseTexturePtr> getActiveTextures() const override;
  bool hasTexture(const BaseTexturePtr& texture) const override;
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  json serialize() const;

  // Statics
  static std::unique_ptr<ShaderMaterial> Parse(const json& source, Scene* scene,
                                               const std::string& url);

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
  std::unordered_map<std::string, TexturePtr> _textures;
  std::unordered_map<std::string, std::vector<BaseTexturePtr>> _textureArrays;
  std::unordered_map<std::string, float> _floats;
  std::unordered_map<std::string, int> _ints;
  std::unordered_map<std::string, Float32Array> _floatsArrays;
  std::unordered_map<std::string, Color3> _colors3;
  std::unordered_map<std::string, Float32Array> _colors3Arrays;
  std::unordered_map<std::string, Color4> _colors4;
  std::unordered_map<std::string, Vector2> _vectors2;
  std::unordered_map<std::string, Vector3> _vectors3;
  std::unordered_map<std::string, Vector4> _vectors4;
  std::unordered_map<std::string, Matrix> _matrices;
  std::unordered_map<std::string, Float32Array> _matrices3x3;
  std::unordered_map<std::string, Float32Array> _matrices2x2;
  std::unordered_map<std::string, Float32Array> _vectors2Arrays;
  std::unordered_map<std::string, Float32Array> _vectors3Arrays;
  Matrix _cachedWorldViewMatrix;
  int _renderId;

}; // end of class ShaderMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_SHADER_MATERIAL_H
