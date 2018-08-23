#ifndef BABYLON_PROBES_REFLECTION_PROBE_H
#define BABYLON_PROBES_REFLECTION_PROBE_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ReflectionProbe {

public:
  template <typename... Ts>
  static ReflectionProbe* New(Ts&&... args)
  {
    auto reflectionProbe = new ReflectionProbe(std::forward<Ts>(args)...);
    reflectionProbe->addToScene(
      static_cast<unique_ptr_t<ReflectionProbe>>(reflectionProbe));

    return reflectionProbe;
  }
  virtual ~ReflectionProbe();

  void addToScene(unique_ptr_t<ReflectionProbe>&& newReflectionProbe);

  Scene* getScene() const;
  RenderTargetTexture* cubeTexture();
  void attachToMesh(AbstractMesh* mesh);

  /**
   * @brief Specifies whether or not the stencil and depth buffer are cleared
   * between two rendering groups.
   * @param renderingGroupId The rendering group id corresponding to its index
   * @param autoClearDepthStencil Automatically clears depth and stencil between
   * groups if true.
   */
  void setRenderingAutoClearDepthStencil(unsigned int renderingGroupId,
                                         bool autoClearDepthStencil);

  void dispose();

protected:
  ReflectionProbe(const string_t& name, const ISize& size, Scene* scene,
                  bool generateMipMaps = true);

private:
  unsigned int get_samples() const;
  void set_samples(unsigned int value);
  int get_refreshRate() const;
  void set_refreshRate(int value);
  vector_t<AbstractMeshPtr>& get_renderList();

public:
  Vector3 position;
  Property<ReflectionProbe, unsigned int> samples;
  Property<ReflectionProbe, int> refreshRate;
  ReadOnlyProperty<ReflectionProbe, vector_t<AbstractMeshPtr>> renderList;

private:
  Scene* _scene;
  RenderTargetTexturePtr _renderTargetTexture;
  Matrix _projectionMatrix;
  Matrix _viewMatrix;
  Vector3 _target;
  Vector3 _add;
  AbstractMesh* _attachedMesh;
  bool _invertYAxis;

}; // end of class ReflectionProbe

} // end of namespace BABYLON

#endif // end of BABYLON_PROBES_REFLECTION_PROBE_H
