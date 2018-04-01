#ifndef BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
#define BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H

#include <babylon/babylon_global.h>
#include <babylon/engine/engine_constants.h>

namespace BABYLON {

/**
 * @brief This renderer is helpfull to fill one of the render target with a
 * geometry buffer.
 */
class BABYLON_SHARED_EXPORT GeometryBufferRenderer {

public:
  /**
   * @brief Creates a new G Buffer for the scene. @see GeometryBufferRenderer
   * @param scene The scene the buffer belongs to
   * @param ratio How big is the buffer related to the main canvas.
   */
  GeometryBufferRenderer(Scene* scene, float ratio = 1.f);
  virtual ~GeometryBufferRenderer();

  /**
   * @brief Set the render list (meshes to be rendered) used in the G buffer.
   */
  void setRenderList(const vector_t<Mesh*>& meshes);

  /**
   * @brief Gets wether or not G buffer are supported by the running hardware.
   * This requires draw buffer supports
   */
  bool isSupported() const;

  /**
   * @brief Gets wether or not position are enabled for the G buffer.
   */
  bool enablePosition() const;

  /**
   * @brief Sets wether or not position are enabled for the G buffer.
   */
  void setEnablePosition(bool enable);

  /**
   * @brief Gets the scene associated with the buffer.
   */
  Scene* scene() const;

  /**
   * @brief Gets the ratio used by the buffer during its creation.
   * How big is the buffer related to the main canvas.
   */
  float ratio() const;

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
  MultiRenderTarget* getGBuffer() const;

  /**
   * @brief Gets the number of samples used to render the buffer (anti
   * aliasing).
   */
  unsigned int samples() const;

  /**
   * @brief Sets the number of samples used to render the buffer (anti
   * aliasing).
   */
  void setSamples(unsigned int value);

  /**
   * @brief Disposes the renderer and frees up associated resources.
   */
  void dispose();

protected:
  void _createRenderTargets();

private:
  /**
   * @brief Custom render function.
   * @param subMesh
   */
  void renderSubMesh(SubMesh* subMesh);

protected:
  Effect* _effect;
  string_t _cachedDefines;

private:
  Scene* _scene;
  unique_ptr_t<MultiRenderTarget> _multiRenderTarget;
  float _ratio;
  bool _enablePosition;

}; // end of class GeometryBufferRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_GEOMETRY_BUFFER_RENDERER_H
