#ifndef BABYLON_MATERIALS_SHADER_MATERIAL_H
#define BABYLON_MATERIALS_SHADER_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/ishader_material_options.h>
#include <babylon/materials/material.h>
#include <babylon/math/color3.h>
#include <babylon/math/color4.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector2.h>
#include <babylon/math/vector3.h>
#include <babylon/math/vector4.h>
#include <unordered_map>

namespace BABYLON {

class ShaderMaterial;
class Texture;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;
using TexturePtr        = std::shared_ptr<Texture>;

/**
 * @brief The ShaderMaterial object has the necessary methods to pass data from
 * your scene to the Vertex and Fragment Shaders and returns a material that can
 * be applied to any mesh.
 *
 * This returned material effects how the mesh will look based on the code in
 * the shaders.
 *
 * @see http://doc.babylonjs.com/how_to/shader_material
 */
class BABYLON_SHARED_EXPORT ShaderMaterial : public Material {

public:
  template <typename... Ts>
  static ShaderMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<ShaderMaterial>(new ShaderMaterial(std::forward<Ts>(args)...));
    material->addMaterialToScene(material);

    return material;
  }
  ~ShaderMaterial() override; // = default

  /**
   * @brief Gets the options used to compile the shader.
   * They can be modified to trigger a new compilation
   */
  IShaderMaterialOptions& options();

  /**
   * @brief Gets the current class name of the material e.g. "ShaderMaterial"
   * Mainly use in serialization.
   * @returns the class name
   */
  const std::string getClassName() const override;

  Type type() const override;

  /**
   * @brief Specifies if the material will require alpha blending.
   * @returns a boolean specifying if alpha blending is needed
   */
  bool needAlphaBlending() const override;

  /**
   * @brief Specifies if this material should be rendered in alpha test mode
   * @returns a boolean specifying if an alpha test is needed.
   */
  bool needAlphaTesting() const override;

  /**
   * @brief Set a texture in the shader.
   * @param name Define the name of the uniform samplers as defined in the
   * shader
   * @param texture Define the texture to bind to this sampler
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setTexture(const std::string& name, const TexturePtr& texture);

  /**
   * @brief Set a texture array in the shader.
   * @param name Define the name of the uniform sampler array as defined in the
   * shader
   * @param textures Define the list of textures to bind to this sampler
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setTextureArray(const std::string& iName,
                                  const std::vector<BaseTexturePtr>& textures);

  /**
   * @brief Set a int in the shader.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setInt(const std::string& name, int value);

  /**
   * @brief Set a float in the shader.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setFloat(const std::string& name, float value);

  /**
   * @brief Set an array of floats in the shader.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setFloats(const std::string& name, const Float32Array& value);

  /**
   * @brief Set a vec3 in the shader from a Color3.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setColor3(const std::string& name, const Color3& value);

  /**
   * @brief Set a vec3 array in the shader from a Color3 array.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setColor3Array(const std::string& iName, const std::vector<Color3>& value);

  /**
   * @brief Set a vec4 in the shader from a Color4.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setColor4(const std::string& name, const Color4& value);

  /**
   * @brief Set a vec4 array in the shader from a Color4 array.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setColor4Array(const std::string& iName, const std::vector<Color4>& value);

  /**
   * @brief Set a vec2 in the shader from a Vector2.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setVector2(const std::string& name, const Vector2& value);

  /**
   * @brief Set a vec3 in the shader from a Vector3.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setVector3(const std::string& name, const Vector3& value);

  /**
   * @brief Set a vec4 in the shader from a Vector4.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setVector4(const std::string& name, const Vector4& value);

  /**
   * @brief Set a mat4 in the shader from a Matrix.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setMatrix(const std::string& name, const Matrix& value);

  /**
   * @brief Set a mat3 in the shader from a Float32Array.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setMatrix3x3(const std::string& name, const Float32Array& value);

  /**
   * @brief Set a mat2 in the shader from a Float32Array.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setMatrix2x2(const std::string& name, const Float32Array& value);

  /**
   * @brief Set a vec2 array in the shader from a number array.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setArray2(const std::string& iName, const Float32Array& value);

  /**
   * @brief Set a vec3 array in the shader from a number array.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setArray3(const std::string& iName, const Float32Array& value);

  /**
   * @brief Set a vec4 array in the shader from a number array.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the material itself allowing "fluent" like uniform updates
   */
  ShaderMaterial& setArray4(const std::string& iName, const Float32Array& value);

  /**
   * @brief Specifies that the submesh is ready to be used.
   * @param mesh defines the mesh to check
   * @param subMesh defines which submesh to check
   * @param useInstances specifies that instances should be used
   * @returns a boolean indicating that the submesh is ready or not
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;

  /**
   * @brief Checks if the material is ready to render the requested mesh.
   * @param mesh Define the mesh to render
   * @param useInstances Define whether or not the material is used with
   * instances
   * @returns true if ready, otherwise false
   */
  bool isReady(AbstractMesh* mesh = nullptr, bool useInstances = false) override;

  /**
   * @brief Binds the world matrix to the material.
   * @param world defines the world transformation matrix
   */
  void bindOnlyWorldMatrix(Matrix& world) override;

  /**
   * @brief Binds the material to the mesh.
   * @param world defines the world transformation matrix
   * @param mesh defines the mesh to bind the material to
   */
  void bind(Matrix& world, Mesh* mesh = nullptr) override;

  /**
   * @brief Gets the active textures from the material.
   * @returns an array of textures
   */
  std::vector<BaseTexturePtr> getActiveTextures() const override;

  /**
   * @brief Specifies if the material uses a texture.
   * @param texture defines the texture to check against the material
   * @returns a boolean specifying if the material uses the texture
   */
  bool hasTexture(const BaseTexturePtr& texture) const override;

  /**
   * @brief Makes a duplicate of the material, and gives it a new name.
   * @param name defines the new name for the duplicated material
   * @returns the cloned material
   */
  MaterialPtr clone(const std::string& name, bool cloneChildren = false) const override;

  /**
   * @brief Disposes the material.
   * @param forceDisposeEffect specifies if effects should be forcefully
   * disposed
   * @param forceDisposeTextures specifies if textures should be forcefully
   * disposed
   * @param notBoundToMesh specifies if the material that is being disposed is
   * known to be not bound to any mesh
   */
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool notBoundToMesh = false) override;

  /**
   * @brief Serializes this material in a JSON representation.
   * @returns the serialized material object
   */
  json serialize() const;

  /**
   * @brief Creates a shader material from parsed shader material data.
   * @param source defines the JSON represnetation of the material
   * @param scene defines the hosting scene
   * @param rootUrl defines the root URL to use to load textures and relative
   * dependencies
   * @returns a new material
   */
  static std::unique_ptr<ShaderMaterial> Parse(const json& source, Scene* scene,
                                               const std::string& url);

protected:
  /**
   * @brief Instantiate a new shader material.
   * The ShaderMaterial object has the necessary methods to pass data from your
   * scene to the Vertex and Fragment Shaders and returns a material that can be
   * applied to any mesh. This returned material effects how the mesh will look
   * based on the code in the shaders.
   * @see http://doc.babylonjs.com/how_to/shader_material
   * @param name Define the name of the material in the scene
   * @param scene Define the scene the material belongs to
   * @param shaderPath Defines  the route to the shader code in one of three
   * ways:
   *  * object: { vertex: "custom", fragment: "custom" }, used with
   * Effect.ShadersStore["customVertexShader"] and
   * Effect.ShadersStore["customFragmentShader"]
   *  * object: { vertexElement: "vertexShaderCode", fragmentElement:
   * "fragmentShaderCode" }, used with shader code in script tags
   *  * object: { vertexSource: "vertex shader code string", fragmentSource:
   * "fragment shader code string" } using with strings containing the shaders
   * code
   *  * string: "./COMMON_NAME", used with external files COMMON_NAME.vertex.fx
   * and COMMON_NAME.fragment.fx in index.html folder.
   * @param options Define the options used to create the shader
   */
  ShaderMaterial(const std::string& name, Scene* scene, const std::string& shaderPath,
                 const IShaderMaterialOptions& options);

private:
  bool _checkCache(AbstractMesh* mesh, bool useInstances = false);
  void _checkUniform(const std::string& uniformName);

private:
  std::string _shaderPath;
  IShaderMaterialOptions _options;
  std::unordered_map<std::string, TexturePtr> _textures;
  std::unordered_map<std::string, std::vector<BaseTexturePtr>> _textureArrays;
  std::unordered_map<std::string, float> _floats;
  std::unordered_map<std::string, int> _ints;
  std::unordered_map<std::string, Float32Array> _floatsArrays;
  std::unordered_map<std::string, Color3> _colors3;
  std::unordered_map<std::string, Float32Array> _colors3Arrays;
  std::unordered_map<std::string, Color4> _colors4;
  std::unordered_map<std::string, Float32Array> _colors4Arrays;
  std::unordered_map<std::string, Vector2> _vectors2;
  std::unordered_map<std::string, Vector3> _vectors3;
  std::unordered_map<std::string, Vector4> _vectors4;
  std::unordered_map<std::string, Matrix> _matrices;
  std::unordered_map<std::string, Float32Array> _matrices3x3;
  std::unordered_map<std::string, Float32Array> _matrices2x2;
  std::unordered_map<std::string, Float32Array> _vectors2Arrays;
  std::unordered_map<std::string, Float32Array> _vectors3Arrays;
  std::unordered_map<std::string, Float32Array> _vectors4Arrays;
  Matrix _cachedWorldViewMatrix;
  Matrix _cachedWorldViewProjectionMatrix;
  int _renderId;

}; // end of class ShaderMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_SHADER_MATERIAL_H
