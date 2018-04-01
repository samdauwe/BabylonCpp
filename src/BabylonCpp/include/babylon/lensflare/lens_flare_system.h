#ifndef BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H
#define BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT LensFlareSystem {

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

  Scene* getScene();
  Mesh* getEmitter();
  void setEmitter(Mesh* newEmitter);
  Vector3 getEmitterPosition();
  bool computeEffectivePosition(Viewport& globalViewport);
  bool _isVisible();
  bool render();
  void dispose();
  Json::object serialize() const;

  // Statics
  static LensFlareSystem* Parse(const Json::value& parsedLensFlareSystem,
                                Scene* scene, const string_t& rootUrl);

protected:
  LensFlareSystem(const string_t name, Mesh* emitter, Scene* scene);

private:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);

public:
  string_t id;
  string_t name;
  vector_t<unique_ptr_t<LensFlare>> lensFlares;
  int borderLimit;
  float viewportBorder;
  ::std::function<bool(AbstractMesh* mesh)> meshesSelectionPredicate;
  unsigned int layerMask;
  Property<LensFlareSystem, bool> isEnabled;

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
