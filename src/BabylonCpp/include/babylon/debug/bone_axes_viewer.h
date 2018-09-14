#ifndef BABYLON_DEBUG_BONE_AXES_VIEWER_H
#define BABYLON_DEBUG_BONE_AXES_VIEWER_H

#include <babylon/babylon_api.h>
#include <babylon/debug/axes_viewer.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class Bone;
class Mesh;

namespace Debug {

/**
 * @brief The BoneAxesViewer will attach 3 axes to a specific bone of a specific
 * mesh.
 * @see demo here: https://www.babylonjs-playground.com/#0DE8F4#8
 */
class BABYLON_SHARED_EXPORT BoneAxesViewer : public AxesViewer {

public:
  /**
   * @brief Creates a new BoneAxesViewer.
   * @param scene defines the hosting scene
   * @param bone defines the target bone
   * @param mesh defines the target mesh
   * @param scaleLines defines a scaling factor for line length (1 by default)
   */
  BoneAxesViewer(Scene* scene, Bone* bone, Mesh* mesh, float scaleLines = 1.f);
  ~BoneAxesViewer();

  /**
   * @brief Force the viewer to update.
   */
  void update();

  /**
   * @brief Releases resources.
   */
  void dispose();

public:
  /**
   * Gets or sets the target mesh where to display the axes viewer
   */
  Mesh* mesh;

  /**
   * Gets or sets the target bone where to display the axes viewer
   */
  Bone* bone;

  /**
   * Gets current position
   */
  Vector3 pos;

  /**
   * Gets direction of X axis
   */
  Vector3 xaxis;

  /**
   * Gets direction of Y axis
   */
  Vector3 yaxis;

  /**
   * Gets direction of Z axis
   */
  Vector3 zaxis;

}; // end of class BoneAxesViewer

} // end of namespace Debug
} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_BONE_AXES_VIEWER_H
