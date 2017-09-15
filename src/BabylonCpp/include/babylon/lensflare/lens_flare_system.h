#ifndef BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H
#define BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT LensFlareSystem : public IDisposable {

public:
  template <typename... Ts>
  static LensFlareSystem* New(Ts&&... /*args*/)
  {
#if 0
    auto lensFlareSystem = new LensFlareSystem(std::forward<Ts>(args)...);
    lensFlareSystem->addToScene(
      static_cast<unique_ptr_t<LensFlareSystem>>(lensFlareSystem));
    return lensFlareSystem;
#endif
    return nullptr;
  }
  virtual ~LensFlareSystem();

  void addToScene(unique_ptr_t<LensFlareSystem>&& lensFlareSystem);

  bool isEnabled() const;
  void setIsEnabled(bool value);
  Scene* getScene();
  Mesh* getEmitter();
  void setEmitter(Mesh* newEmitter);
  Vector3 getEmitterPosition();
  bool computeEffectivePosition(Viewport& globalViewport);
  bool _isVisible();
  bool render();
  void dispose(bool doNotRecurse = false) override;
  Json::object serialize() const;

  // Statics
  static LensFlareSystem* Parse(const Json::value& parsedLensFlareSystem,
                                Scene* scene, const string_t& rootUrl);

protected:
  LensFlareSystem(const string_t name, Mesh* emitter, Scene* scene);

public:
  string_t id;
  string_t name;
  vector_t<unique_ptr_t<LensFlare>> lensFlares;
  int borderLimit;
  float viewportBorder;
  ::std::function<bool(Mesh* mesh)> meshesSelectionPredicate;
  unsigned int layerMask;

private:
  Scene* _scene;
  Mesh* _emitter;
  // Float32Array _vertexDeclaration;
  // int _vertexStrideSize;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  float _positionX;
  float _positionY;
  bool _isEnabled;

}; // end of class LensFlareSystem

} // end of namespace BABYLON

#endif // end of BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H
