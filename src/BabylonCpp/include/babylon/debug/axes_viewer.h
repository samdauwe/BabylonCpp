#ifndef BABYLON_DEBUG_AXES_VIEWER_H
#define BABYLON_DEBUG_AXES_VIEWER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT AxesViewer {

public:
  AxesViewer(Scene* scene, float scaleLines = 1);
  ~AxesViewer();

  void update(const Vector3& position, const Vector3& xaxis,
              const Vector3& yaxis, const Vector3& zaxis);
  void dispose();

public:
  Scene* scene;
  float scaleLines;

private:
  std::vector<Vector3> _xline;
  std::vector<Vector3> _yline;
  std::vector<Vector3> _zline;

  LinesMesh* _xmesh;
  LinesMesh* _ymesh;
  LinesMesh* _zmesh;

}; // end of class AxesViewer

} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_AXES_VIEWER_H