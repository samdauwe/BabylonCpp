#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

class Effect;
class Engine;
class ProceduralTexture;
class VertexBuffer;
using ProceduralTexturePtr = std::shared_ptr<ProceduralTexture>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

class BABYLON_SHARED_EXPORT ProceduralTexture : public Texture {

  friend class CustomProceduralTexture;

public:
  template <typename... Ts>
  static ProceduralTexturePtr New(Ts&&... args)
  {
    auto texture = std::shared_ptr<ProceduralTexture>(
      new ProceduralTexture(::std::forward<Ts>(args)...));
    texture->addToScene(texture);

    return texture;
  }
  ~ProceduralTexture() override;

  void addToScene(const ProceduralTexturePtr& newTexture);

  /** Hidden */
  void _rebuild() override;
  void reset();
  bool isReady() override;
  void resetRefreshCounter();
  void
  setFragment(const std::unordered_map<std::string, std::string>& fragment);
  void setFragment(const std::string& fragment);
  /** Hidden */
  bool _shouldRender();
  Size& getRenderSize();
  const Size& getRenderSize() const;
  void resize(const Size& size, bool generateMipMaps);
  void _checkUniform(const std::string& uniformName);
  ProceduralTexture& setTexture(const std::string& name,
                                const TexturePtr& texture);
  ProceduralTexture& setFloat(const std::string& name, float value);

  /**
   * @brief Set the value of an uniform to an integer value.
   * @param name defines the name of the uniform
   * @param value defines the value to set
   * @returns the current procedural texture
   */
  ProceduralTexture& setInt(const std::string& name, int value);

  ProceduralTexture& setFloats(const std::string& name, Float32Array value);
  ProceduralTexture& setColor3(const std::string& name, const Color3& value);
  ProceduralTexture& setColor4(const std::string& name, const Color4& value);
  ProceduralTexture& setVector2(const std::string& name, const Vector2& value);
  ProceduralTexture& setVector3(const std::string& name, const Vector3& value);
  ProceduralTexture& setMatrix(const std::string& name, const Matrix& value);
  virtual void render(bool useCameraPostProcess = false);
  ProceduralTexturePtr clone();
  void dispose() override;

protected:
  ProceduralTexture(
    const std::string& name, const Size& size,
    const std::unordered_map<std::string, std::string>& fragment, Scene* scene,
    Texture* fallbackTexture = nullptr, bool generateMipMaps = true,
    bool isCube = false);
  ProceduralTexture(const std::string& name, const Size& size,
                    const std::string& fragment, Scene* scene,
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
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  std::vector<std::string> _uniforms;
  std::vector<std::string> _samplers;
  std::unordered_map<std::string, std::string> _fragment;
  /** Hidden */
  std::unordered_map<std::string, TexturePtr> _textures;
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

}; // end of class ProceduralTexture

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
