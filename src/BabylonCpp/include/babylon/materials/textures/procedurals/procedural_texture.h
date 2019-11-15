#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

class Effect;
class Engine;
class ProceduralTexture;
class VertexBuffer;
using EffectPtr            = std::shared_ptr<Effect>;
using ProceduralTexturePtr = std::shared_ptr<ProceduralTexture>;
using VertexBufferPtr      = std::shared_ptr<VertexBuffer>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief Procedural texturing is a way to programmatically create a texture.
 * There are 2 types of procedural textures: code-only, and code that references
 * some classic 2D images, sometimes calmpler' images. This is the base class of
 * any Procedural texture and contains most of the shareable code.
 * @see http://doc.babylonjs.com/how_to/how_to_use_procedural_textures
 */
class BABYLON_SHARED_EXPORT ProceduralTexture : public Texture {

  friend class CustomProceduralTexture;

public:
  template <typename... Ts>
  static ProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<ProceduralTexture>(
      new ProceduralTexture(std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~ProceduralTexture(); // = default

  void addToScene(const ProceduralTexturePtr& newTexture);

  /**
   * @brief The effect that is created when initializing the post process.
   * @returns The created effect corresponding the the postprocess.
   */
  EffectPtr& getEffect();

  /**
   * @brief Gets texture content (Use this function wisely as reading from a
   * texture can be slow).
   * @returns an ArrayBufferView (Uint8Array or Float32Array)
   */
  ArrayBufferView& getContent();

  /**
   * @brief Hidden
   */
  void _rebuild() override;

  /**
   * @brief Resets the texture in order to recreate its associated resources.
   * This can be called in case of context loss
   */
  void reset();

  /**
   * @brief Is the texture ready to be used ? (rendered at least once).
   * @returns true if ready, otherwise, false.
   */
  bool isReady() override;

  /**
   * @brief Resets the refresh counter of the texture and start bak from
   * scratch. Could be usefull to regenerate the texture if it is setup to
   * render only once.
   */
  void resetRefreshCounter();

  /**
   * @brief Set the fragment shader to use in order to render the texture.
   * @param fragment This can be set to a path (into the shader store) or to a
   * json object containing a fragmentElement property.
   */
  void
  setFragment(const std::unordered_map<std::string, std::string>& fragment);

  /**
   * @brief Set the fragment shader to use in order to render the texture.
   * @param fragment This can be set to a path (into the shader store) or to a
   * json object containing a fragmentElement property.
   */
  void setFragment(const std::string& fragment);

  /**
   * @brief Hidden
   */
  bool _shouldRender();

  /**
   * @brief Get the size the texture is rendering at.
   * @returns the size (texture is always squared)
   */
  Size& getRenderSize();

  /**
   * @brief Get the size the texture is rendering at.
   * @returns the size (texture is always squared)
   */
  const Size& getRenderSize() const;

  /**
   * @brief Resize the texture to new value.
   * @param size Define the new size the texture should have
   * @param generateMipMaps Define whether the new texture should create mip
   * maps
   */
  void resize(const Size& size, bool generateMipMaps);

  /**
   * @brief Set a texture in the shader program used to render.
   * @param name Define the name of the uniform samplers as defined in the
   * shader
   * @param texture Define the texture to bind to this sampler
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setTexture(const std::string& name,
                                const TexturePtr& texture);

  /**
   * @brief Set a float in the shader.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setFloat(const std::string& name, float value);

  /**
   * @brief Set a int in the shader.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setInt(const std::string& name, int value);

  /**
   * @brief Set an array of floats in the shader.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setFloats(const std::string& name, Float32Array value);

  /**
   * @brief Set a vec3 in the shader from a Color3.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setColor3(const std::string& name, const Color3& value);

  /**
   * @brief Set a vec4 in the shader from a Color4.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setColor4(const std::string& name, const Color4& value);

  /**
   * @brief Set a vec2 in the shader from a Vector2.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setVector2(const std::string& name, const Vector2& value);

  /**
   * @brief Set a vec3 in the shader from a Vector3.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setVector3(const std::string& name, const Vector3& value);

  /**
   * @brief Set a mat4 in the shader from a Matrix.
   * @param name Define the name of the uniform as defined in the shader
   * @param value Define the value to give to the uniform
   * @return the texture itself allowing "fluent" like uniform updates
   */
  ProceduralTexture& setMatrix(const std::string& name, const Matrix& value);

  /**
   * @brief Render the texture to its associated render target.
   * @param useCameraPostProcess Define if camera post process should be applied
   * to the texture
   */
  virtual void render(bool useCameraPostProcess = false);

  /**
   * @brief Clone the texture.
   * @returns the cloned texture
   */
  ProceduralTexturePtr clone();

  /**
   * @brief Dispose the texture and release its asoociated resources.
   */
  void dispose() override;

protected:
  /**
   * @brief Instantiates a new procedural texture.
   * Procedural texturing is a way to programmatically create a texture. There
   * are 2 types of procedural textures: code-only, and code that references
   * some classic 2D images, sometimes called 'refMaps' or 'sampler' images.
   * This is the base class of any Procedural texture and contains most of the
   * shareable code.
   * @see http://doc.babylonjs.com/how_to/how_to_use_procedural_textures
   * @param name  Define the name of the texture
   * @param size Define the size of the texture to create
   * @param fragment Define the fragment shader to use to generate the texture
   * or null if it is defined later
   * @param scene Define the scene the texture belongs to
   * @param fallbackTexture Define a fallback texture in case there were issues
   * to create the custom texture
   * @param generateMipMaps Define if the texture should creates mip maps or not
   * @param isCube Define if the texture is a cube texture or not (this will
   * render each faces of the cube)
   */
  ProceduralTexture(
    const std::string& name, const Size& size,
    const std::unordered_map<std::string, std::string>& fragment, Scene* scene,
    Texture* fallbackTexture = nullptr, bool generateMipMaps = true,
    bool isCube = false);
  ProceduralTexture(const std::string& name, const Size& size,
                    const std::string& fragment, Scene* scene,
                    Texture* fallbackTexture = nullptr,
                    bool generateMipMaps = true, bool isCube = false);

  /**
   * @brief Define the refresh rate of the texture or the rendering frequency.
   * Use 0 to render just once, 1 to render on every frame, 2 to render every
   * two frames and so on...
   */
  float get_refreshRate() const;

  /**
   * @brief Define the refresh rate of the texture or the rendering frequency.
   * Use 0 to render just once, 1 to render on every frame, 2 to render every
   * two frames and so on...
   */
  void set_refreshRate(float value);

  virtual std::string _getDefines() const;

private:
  void _createIndexBuffer();
  void _checkUniform(const std::string& uniformName);

public:
  /**
   * Define if the texture is enabled or not (disabled texture will not render)
   */
  bool isEnabled;

  /**
   * Define if the texture must be cleared before rendering (default is true)
   */
  bool autoClear;

  /**
   * Callback called when the texture is generated
   */
  std::function<void()> onGenerated;

  /**
   * Event raised when the texture is generated
   */
  Observable<ProceduralTexture> onGeneratedObservable;

  /**
   * Hidden
   */
  bool _generateMipMaps;

  /**
   * Hidden
   */
  EffectPtr _effect;

  /**
   * Hidden
   */
  std::unordered_map<std::string, TexturePtr> _textures;

  /**
   * Define the refresh rate of the texture or the rendering frequency.
   * Use 0 to render just once, 1 to render on every frame, 2 to render every
   * two frames and so on...
   */
  Property<ProceduralTexture, float> refreshRate;

private:
  Size _size;
  bool _doNotChangeAspectRatio;
  int _currentRefreshId;
  float _refreshRate;
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  std::vector<std::string> _uniforms;
  std::vector<std::string> _samplers;
  std::unordered_map<std::string, std::string> _fragment;
  std::unordered_map<std::string, float> _floats;
  std::unordered_map<std::string, int> _ints;
  std::unordered_map<std::string, Float32Array> _floatsArrays;
  std::unordered_map<std::string, Color3> _colors3;
  std::unordered_map<std::string, Color4> _colors4;
  std::unordered_map<std::string, Vector2> _vectors2;
  std::unordered_map<std::string, Vector3> _vectors3;
  std::unordered_map<std::string, Matrix> _matrices;
  Texture* _fallbackTexture;
  bool _fallbackTextureUsed;
  Engine* _engine;
  std::string _cachedDefines;
  int _contentUpdateId;
  ArrayBufferView _contentData;

}; // end of class ProceduralTexture

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
