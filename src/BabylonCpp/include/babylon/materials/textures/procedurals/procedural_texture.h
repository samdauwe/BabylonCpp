#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ProceduralTexture : public Texture {

  friend class CustomProceduralTexture;

public:
  template <typename... Ts>
  static ProceduralTexture* New(Ts&&... args)
  {
    auto texture = new ProceduralTexture(::std::forward<Ts>(args)...);
    texture->addToScene(static_cast<unique_ptr_t<ProceduralTexture>>(texture));

    return texture;
  }
  ~ProceduralTexture() override;

  void addToScene(unique_ptr_t<ProceduralTexture>&& newTexture);

  /** Hidden */
  void _rebuild() override;
  void reset();
  bool isReady() override;
  void resetRefreshCounter();
  void setFragment(const unordered_map_t<string_t, string_t>& fragment);
  void setFragment(const string_t& fragment);
  /** Hidden */
  bool _shouldRender();
  Size& getRenderSize();
  const Size& getRenderSize() const;
  void resize(const Size& size, bool generateMipMaps);
  void _checkUniform(const string_t& uniformName);
  ProceduralTexture& setTexture(const string_t& name,
                                const TexturePtr& texture);
  ProceduralTexture& setFloat(const string_t& name, float value);

  /**
   * @brief Set the value of an uniform to an integer value.
   * @param name defines the name of the uniform
   * @param value defines the value to set
   * @returns the current procedural texture
   */
  ProceduralTexture& setInt(const string_t& name, int value);

  ProceduralTexture& setFloats(const string_t& name, Float32Array value);
  ProceduralTexture& setColor3(const string_t& name, const Color3& value);
  ProceduralTexture& setColor4(const string_t& name, const Color4& value);
  ProceduralTexture& setVector2(const string_t& name, const Vector2& value);
  ProceduralTexture& setVector3(const string_t& name, const Vector3& value);
  ProceduralTexture& setMatrix(const string_t& name, const Matrix& value);
  virtual void render(bool useCameraPostProcess = false);
  ProceduralTexture* clone();
  void dispose() override;

protected:
  ProceduralTexture(const string_t& name, const Size& size,
                    const unordered_map_t<string_t, string_t>& fragment,
                    Scene* scene, Texture* fallbackTexture = nullptr,
                    bool generateMipMaps = true, bool isCube = false);
  ProceduralTexture(const string_t& name, const Size& size,
                    const string_t& fragment, Scene* scene,
                    Texture* fallbackTexture = nullptr,
                    bool generateMipMaps = true, bool isCube = false);

  int get_refreshRate() const;

  /**
   * @brief Use 0 to render just once, 1 to render on every frame, 2 to render
   * every two frames and so on...
   * @param value
   */
  void set_refreshRate(int value);

private:
  void _createIndexBuffer();

public:
  /** Hidden */
  bool _generateMipMaps;
  bool isEnabled;
  ::std::function<void()> onGenerated;

  Property<ProceduralTexture, int> refreshRate;

private:
  Size _size;
  bool _doNotChangeAspectRatio;
  int _currentRefreshId;
  int _refreshRate;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  vector_t<string_t> _uniforms;
  vector_t<string_t> _samplers;
  unordered_map_t<string_t, string_t> _fragment;
  /** Hidden */
  unordered_map_t<string_t, TexturePtr> _textures;
  unordered_map_t<string_t, float> _floats;
  unordered_map_t<string_t, int> _ints;
  unordered_map_t<string_t, Float32Array> _floatsArrays;
  unordered_map_t<string_t, Color3> _colors3;
  unordered_map_t<string_t, Color4> _colors4;
  unordered_map_t<string_t, Vector2> _vectors2;
  unordered_map_t<string_t, Vector3> _vectors3;
  unordered_map_t<string_t, Matrix> _matrices;
  Texture* _fallbackTexture;
  bool _fallbackTextureUsed;
  Engine* _engine;

}; // end of class ProceduralTexture

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
