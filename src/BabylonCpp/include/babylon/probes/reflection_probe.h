#ifndef BABYLON_PROBES_REFLECTION_PROBE_H
#define BABYLON_PROBES_REFLECTION_PROBE_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ReflectionProbe : IDisposable {

public:
  template <typename... Ts>
  static ReflectionProbe* New(Ts&&... args)
  {
    auto reflectionProbe = new ReflectionProbe(std::forward<Ts>(args)...);
    reflectionProbe->addToScene(
      static_cast<std::unique_ptr<ReflectionProbe>>(reflectionProbe));

    return reflectionProbe;
  }
  virtual ~ReflectionProbe();

  void addToScene(std::unique_ptr<ReflectionProbe>&& newReflectionProbe);

  unsigned int samples() const;
  void setSamples(unsigned int value);
  int refreshRate() const;
  void setRefreshRate(int value);
  Scene* getScene() const;
  RenderTargetTexture* cubeTexture();
  std::vector<AbstractMesh*>& renderList();
  void attachToMesh(AbstractMesh* mesh);
  void dispose(bool doNotRecurse = false) override;

protected:
  ReflectionProbe(const std::string& name, const ISize& size, Scene* scene,
                  bool generateMipMaps = true);

public:
  bool invertYAxis;
  Vector3 position;

private:
  Scene* _scene;
  std::unique_ptr<RenderTargetTexture> _renderTargetTexture;
  Matrix _projectionMatrix;
  Matrix _viewMatrix;
  Vector3 _target;
  Vector3 _add;
  AbstractMesh* _attachedMesh;

}; // end of class ReflectionProbe

} // end of namespace BABYLON

#endif // end of BABYLON_PROBES_REFLECTION_PROBE_H
