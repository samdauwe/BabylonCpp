#ifndef BABYLON_DEBUG_AXES_VIEWER_H
#define BABYLON_DEBUG_AXES_VIEWER_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>

namespace BABYLON {

class LinesMesh;
class Scene;
class Vector3;
using LinesMeshPtr = std::shared_ptr<LinesMesh>;

namespace Debug {

/**
 * @brief The Axes viewer will show 3 axes in a specific point in space.
 */
class BABYLON_SHARED_EXPORT AxesViewer {

public:
  /**
   * @brief Creates a new AxesViewer.
   * @param scene defines the hosting scene
   * @param scaleLines defines a number used to scale line length (1 by default)
   */
  AxesViewer(Scene* scene, float scaleLines = 1.f);
  ~AxesViewer();

  /**
   * @brief Force the viewer to update.
   * @param position defines the position of the viewer
   * @param xaxis defines the x axis of the viewer
   * @param yaxis defines the y axis of the viewer
   * @param zaxis defines the z axis of the viewer
   */
  void update(const Vector3& position, const Vector3& xaxis,
              const Vector3& yaxis, const Vector3& zaxis);

  /**
   * @brief Releases resources.
   */
  void dispose();

public:
  /**
   * Gets the hosting scene
   */
  Scene* scene;

  /**
   * Gets or sets a number used to scale line length
   */
  float scaleLines;

private:
  std::vector<Vector3> _xline;
  std::vector<Vector3> _yline;
  std::vector<Vector3> _zline;

  LinesMeshPtr _xmesh;
  LinesMeshPtr _ymesh;
  LinesMeshPtr _zmesh;

}; // end of class AxesViewer

} // end of namespace Debug
} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_AXES_VIEWER_H
