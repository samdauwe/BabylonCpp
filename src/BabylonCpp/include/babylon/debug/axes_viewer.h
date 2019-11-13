#ifndef BABYLON_DEBUG_AXES_VIEWER_H
#define BABYLON_DEBUG_AXES_VIEWER_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Scene;
class Vector3;
class TransformNode;
using TransformNodePtr = std::shared_ptr<TransformNode>;

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
   * @param renderingGroupId defines a number used to set the renderingGroupId
   * of the meshes (2 by default)
   * @param xAxis defines the node hierarchy used to render the x-axis
   * @param yAxis defines the node hierarchy used to render the y-axis
   * @param zAxis defines the node hierarchy used to render the z-axis
   */
  AxesViewer(Scene* scene, float scaleLines = 1.f,
             const std::optional<int>& renderingGroupId = 2,
             TransformNodePtr xAxis = nullptr, TransformNodePtr yAxis = nullptr,
             TransformNodePtr zAxis = nullptr);
  ~AxesViewer(); // = default

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
   * @brief Creates an instance of this axes viewer.
   * @returns a new axes viewer with instanced meshes
   */
  std::unique_ptr<AxesViewer> createInstance() const;

  /**
   * @brief Releases resources.
   */
  void dispose();

protected:
  /**
   * @brief Gets the node hierarchy used to render x-axis.
   */
  TransformNodePtr& get_xAxis();

  /**
   * @brief Gets the node hierarchy used to render y-axis.
   */
  TransformNodePtr& get_yAxis();

  /**
   * @brief Gets the node hierarchy used to render z-axis.
   */
  TransformNodePtr& get_zAxis();

private:
  static void _SetRenderingGroupId(const TransformNodePtr& node, int id);

public:
  /**
   * Gets the hosting scene
   */
  Scene* scene;

  /**
   * Gets or sets a number used to scale line length
   */
  float scaleLines;

  /**
   * Gets the node hierarchy used to render x-axis
   */
  ReadOnlyProperty<AxesViewer, TransformNodePtr> xAxis;

  /**
   * Gets the node hierarchy used to render y-axis
   */
  ReadOnlyProperty<AxesViewer, TransformNodePtr> yAxis;

  /**
   * Gets the node hierarchy used to render z-axis
   */
  ReadOnlyProperty<AxesViewer, TransformNodePtr> zAxis;

private:
  TransformNodePtr _xAxis;
  TransformNodePtr _yAxis;
  TransformNodePtr _zAxis;
  float _scaleLinesFactor;
  bool _instanced;

}; // end of class AxesViewer

} // end of namespace Debug
} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_AXES_VIEWER_H
