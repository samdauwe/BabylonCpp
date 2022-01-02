#ifndef BABYLON_RENDERING_IEDGES_RENDERER_H
#define BABYLON_RENDERING_IEDGES_RENDERER_H

#include <vector>

#include <babylon/interfaces/idisposable.h>
#include <babylon/maths/matrix.h>

namespace BABYLON {

/**
 * @brief Defines the minimum contract an Edges renderer should follow.
 */
struct BABYLON_SHARED_EXPORT IEdgesRenderer : public IDisposable {
  ~IEdgesRenderer() override = default;
  /**
   * Gets or sets a boolean indicating if the edgesRenderer is active
   */
  bool isEnabled;

  /**
   * @brief Renders the edges of the attached mesh,
   */
  virtual void render() = 0;

  /**
   * @brief Checks whether or not the edges renderer is ready to render.
   * @return true if ready, otherwise false.
   */
  virtual bool isReady() = 0;

  /**
   * List of instances to render in case the source mesh has instances
   */
  std::vector<Matrix> customInstances;

}; // end of struct IEdgesRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_IEDGES_RENDERER_H
