#ifndef BABYLON_DEBUG_SKELETON_VIEWER_H
#define BABYLON_DEBUG_SKELETON_VIEWER_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SkeletonViewer {

public:
  SkeletonViewer(Skeleton* skeleton, AbstractMesh* mesh, Scene* scene,
                 bool autoUpdateBonesMatrices  = true,
                 unsigned int renderingGroupId = 1);
  ~SkeletonViewer();

  void setIsEnabled(bool value);
  bool isEnabled() const;
  void update();
  void dispose();

private:
  void _getBonePosition(Vector3& position, Bone* bone, const Matrix& meshMat,
                        float x = 0.f, float y = 0.f, float z = 0.f) const;
  void
  _getLinesForBonesWithLength(const std::vector<std::unique_ptr<Bone>>& bones,
                              const Matrix& meshMat);
  void _resizeDebugLines(size_t bonesSize);
  void
  _getLinesForBonesNoLength(const std::vector<std::unique_ptr<Bone>>& bones,
                            const Matrix& meshMat);
  void _updateBoneMatrix(Bone* bone);

public:
  Color3 color;
  Skeleton* skeleton;
  AbstractMesh* mesh;
  bool autoUpdateBonesMatrices;
  unsigned int renderingGroupId;

private:
  Scene* _scene;
  std::vector<std::vector<Vector3>> _debugLines;
  LinesMesh* _debugMesh;
  bool _isEnabled;
  std::function<void()> _renderFunction;

}; // end of class SkeletonViewer

} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_SKELETON_VIEWER_H
