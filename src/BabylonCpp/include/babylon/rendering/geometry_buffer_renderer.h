#ifndef BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
#define BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/engine/engine_constants.h>

namespace BABYLON {

class Effect;
class Mesh;
class SubMesh;
class MultiRenderTarget;
class Scene;
class SubMesh;
using EffectPtr            = std::shared_ptr<Effect>;
using MeshPtr              = std::shared_ptr<Mesh>;
using SubMeshPtr           = std::shared_ptr<SubMesh>;
using MultiRenderTargetPtr = std::shared_ptr<MultiRenderTarget>;

/**
 * @brief This renderer is helpfull to fill one of the render target with a
 * geometry buffer.
 */
class BABYLON_SHARED_EXPORT GeometryBufferRenderer {

public:
  /**
   * @brief Creates a new G Buffer for the scene.
   * @param scene The scene the buffer belongs to
   * @param ratio How big is the buffer related to the main canvas.
   */
  GeometryBufferRenderer(Scene* scene, float ratio = 1.f);
  virtual ~GeometryBufferRenderer();

  /**
   * @brief Checks wether everything is ready to render a submesh to the G
   * buffer.
   * @param subMesh the submesh to check readiness for
   * @param useInstances is the mesh drawn using instance or not
   * @returns true if ready otherwise false
   */
  bool isReady(SubMesh* subMesh, bool useInstances);

  /**
   * @brief Gets the current underlying G Buffer.
   * @returns the buffer
   */
  MultiRenderTargetPtr getGBuffer() const;

  /**
   * @brief Disposes the renderer and frees up associated resources.
   */
  void dispose();

protected:
  /**
   * @brief Set the render list (meshes to be rendered) used in the G buffer.
   */
  void set_renderList(const std::vector<MeshPtr>& meshes);

  /**
   * @brief Gets wether or not G buffer are supported by the running hardware.
   * This requires draw buffer supports
   */
  bool get_isSupported() const;

  /**
   * @brief Gets wether or not position are enabled for the G buffer.
   */
  bool get_enablePosition() const;

  /**
   * @brief Sets wether or not position are enabled for the G buffer.
   */
  void set_enablePosition(bool enable);

  /**
   * @brief Gets the scene associated with the buffer.
   */
  Scene*& get_scene();

  /**
   * @brief Gets the ratio used by the buffer during its creation.
   * How big is the buffer related to the main canvas.
   */
  float get_ratio() const;

  /**
   * @brief Gets the number of samples used to render the buffer (anti
   * aliasing).
   */
  unsigned int get_samples() const;

  /**
   * @brief Sets the number of samples used to render the buffer (anti
   * aliasing).
   */
  void set_samples(unsigned int value);

  void _createRenderTargets();

private:
  /**
   * @brief Custom render function.
   * @param subMesh
   */
  void renderSubMesh(const SubMeshPtr& subMesh);

public:
  /**
   * The render list (meshes to be rendered) used in the G buffer.
   */
  WriteOnlyProperty<GeometryBufferRenderer, std::vector<MeshPtr>> renderList;

  /**
   * Wether or not G buffer are supported by the running hardware.
   * This requires draw buffer supports
   */
  ReadOnlyProperty<GeometryBufferRenderer, bool> isSupported;

  /**
   * Wether or not position are enabled for the G buffer.
   */
  Property<GeometryBufferRenderer, bool> enablePosition;

  /**
   * The scene associated with the buffer.
   */
  ReadOnlyProperty<GeometryBufferRenderer, Scene*> scene;

  /**
   * The ratio used by the buffer during its creation.
   * How big is the buffer related to the main canvas.
   */
  ReadOnlyProperty<GeometryBufferRenderer, float> ratio;

  /**
   * The number of samples used to render the buffer (anti aliasing).
   */
  Property<GeometryBufferRenderer, unsigned int> samples;

protected:
  EffectPtr _effect;
  std::string _cachedDefines;

private:
  Scene* _scene;
  MultiRenderTargetPtr _multiRenderTarget;
  float _ratio;
  bool _enablePosition;

}; // end of class GeometryBufferRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
