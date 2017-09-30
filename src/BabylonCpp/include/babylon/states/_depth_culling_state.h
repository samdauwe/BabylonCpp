#ifndef BABYLON_DEPTH_CULLING_STATE_H
#define BABYLON_DEPTH_CULLING_STATE_H

#include <babylon/babylon_global.h>

namespace BABYLON {
namespace Internals {

class BABYLON_SHARED_EXPORT _DepthCullingState {

public:
  /**
   * Initializes the state.
   */
  _DepthCullingState();
  ~_DepthCullingState();

  bool isDirty() const;
  float zOffset() const;
  void setZOffset(float value);
  int cullFace() const;
  void setCullFace(int value);
  bool cull() const;
  void setCull(bool value);
  int depthFunc() const;
  void setDepthFunc(int value);
  bool depthMask() const;
  void setDepthMask(bool value);
  bool depthTest() const;
  void setDepthTest(bool value);
  void reset();
  void apply(GL::IGLRenderingContext& _gl);

private:
  bool _isDepthTestDirty;
  bool _isDepthMaskDirty;
  bool _isDepthFuncDirty;
  bool _isCullFaceDirty;
  bool _isCullDirty;
  bool _isZOffsetDirty;

  bool _depthTest;
  bool _depthMask;
  int _depthFunc;
  bool _cull;
  int _cullFace;
  float _zOffset;

}; // end of class _DepthCullingState

} // end of namespace Internals
} // end of namespace BABYLON

#endif // end of BABYLON_DEPTH_CULLING_STATE_H
