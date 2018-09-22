#ifndef BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H
#define BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H

#include <functional>
#include <map>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace picojson {
class value;
typedef std::vector<value> array;
typedef std::map<std::string, value> object;
} // end of namespace picojson

namespace BABYLON {

class AbstractMesh;
class Effect;
class LensFlare;
class LensFlareSystem;
class Mesh;
class Scene;
class Vector3;
class VertexBuffer;
class Viewport;
using LensFlareSystemPtr = std::shared_ptr<LensFlareSystem>;
using AbstractMeshPtr    = std::shared_ptr<AbstractMesh>;

namespace Json {
typedef picojson::value value;
typedef picojson::array array;
typedef picojson::object object;
} // namespace Json

namespace GL {
class IGLBuffer;
} // end of namespace GL

class BABYLON_SHARED_EXPORT LensFlareSystem
    : public std::enable_shared_from_this<LensFlareSystem> {

public:
  template <typename... Ts>
  static LensFlareSystemPtr New(Ts&&... args)
  {
    return std::shared_ptr<LensFlareSystem>(
      new LensFlareSystem(std::forward<Ts>(args)...));
  }
  virtual ~LensFlareSystem();

  Scene* getScene();
  Mesh* getEmitter();
  void setEmitter(Mesh* newEmitter);
  Vector3 getEmitterPosition();
  bool computeEffectivePosition(Viewport& globalViewport);

  /**
   * @brief Hidden
   */
  bool _isVisible();

  bool render();
  void dispose();
  Json::object serialize() const;

  // Statics
  static LensFlareSystemPtr Parse(const Json::value& parsedLensFlareSystem,
                                  Scene* scene, const std::string& rootUrl);

protected:
  LensFlareSystem(const std::string name, Mesh* emitter, Scene* scene);

private:
  bool get_isEnabled() const;
  void set_isEnabled(bool value);

public:
  std::string id;
  std::string name;
  std::vector<std::unique_ptr<LensFlare>> lensFlares;
  int borderLimit;
  float viewportBorder;
  std::function<bool(const AbstractMeshPtr& mesh)> meshesSelectionPredicate;
  unsigned int layerMask;
  Property<LensFlareSystem, bool> isEnabled;

private:
  Scene* _scene;
  Mesh* _emitter;
  // Float32Array _vertexDeclaration;
  // int _vertexStrideSize;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  float _positionX;
  float _positionY;
  bool _isEnabled;

}; // end of class LensFlareSystem

} // end of namespace BABYLON

#endif // end of BABYLON_LENSFLARE_LENS_FLARE_SYSTEM_H
