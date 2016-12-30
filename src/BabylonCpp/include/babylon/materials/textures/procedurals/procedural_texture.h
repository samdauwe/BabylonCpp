#ifndef BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ProceduralTexture : public Texture {

public:
  ProceduralTexture(
    const std::string& name, const Size& size,
    const std::unordered_map<std::string, std::string>& fragment, Scene* scene,
    Texture* fallbackTexture, bool generateMipMaps = true);
  ProceduralTexture(const std::string& name, const Size& size,
                    const std::string& fragment, Scene* scene,
                    Texture* fallbackTexture, bool generateMipMaps = true);
  ~ProceduralTexture();

  void reset();
  bool isReady() override;
  void resetRefreshCounter();
  void
  setFragment(const std::unordered_map<std::string, std::string>& fragment);
  void setFragment(const std::string& fragment);
  int refreshRate() const;
  void setRefreshRate(int value);
  bool _shouldRender();
  Size& getRenderSize();
  const Size& getRenderSize() const;
  void resize(const Size& size, bool generateMipMaps);
  void _checkUniform(const std::string& uniformName);
  ProceduralTexture& setTexture(const std::string& name, Texture* texture);
  ProceduralTexture& setFloat(const std::string& name, float value);
  ProceduralTexture& setFloats(const std::string& name,
                               std::vector<float> value);
  ProceduralTexture& setColor3(const std::string& name, const Color3& value);
  ProceduralTexture& setColor4(const std::string& name, const Color4& value);
  ProceduralTexture& setVector2(const std::string& name, const Vector2& value);
  ProceduralTexture& setVector3(const std::string& name, const Vector3& value);
  ProceduralTexture& setMatrix(const std::string& name, const Matrix& value);
  void render(bool useCameraPostProcess = false);
  std::unique_ptr<ProceduralTexture> clone() const;
  void dispose(bool doNotRecurse = false) override;

public:
  bool _generateMipMaps;
  bool isEnabled;
  std::function<void()> onGenerated;

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
  std::unordered_map<std::string, Texture*> _textures;
  std::unordered_map<std::string, float> _floats;
  std::unordered_map<std::string, Float32Array> _floatsArrays;
  std::unordered_map<std::string, Color3> _colors3;
  std::unordered_map<std::string, Color4> _colors4;
  std::unordered_map<std::string, Vector2> _vectors2;
  std::unordered_map<std::string, Vector3> _vectors3;
  std::unordered_map<std::string, Matrix> _matrices;
  Texture* _fallbackTexture;
  bool _fallbackTextureUsed;

}; // end of class ProceduralTexture

} // end of namespace BABYLON

#endif // end of
       // BABYLON_MATERIALS_TEXTURES_PROCEDURALS_PROCEDURAL_TEXTURE_H
