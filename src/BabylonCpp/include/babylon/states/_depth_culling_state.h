#ifndef BABYLON_DEPTH_CULLING_STATE_H
#define BABYLON_DEPTH_CULLING_STATE_H

#include <babylon/babylon_global.h>
#include <babylon/core/nullable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT _DepthCullingState {

public:
  /**
   * @brief Initializes the state.
   */
  _DepthCullingState();
  ~_DepthCullingState();

  void reset();
  void apply(GL::IGLRenderingContext& _gl);

protected:
  bool get_isDirty() const;
  float get_zOffset() const;
  void set_zOffset(float value);
  Nullable<int>& get_cullFace();
  void set_cullFace(const Nullable<int>& value);
  Nullable<bool>& get_cull();
  void set_cull(const Nullable<bool>& value);
  Nullable<int>& get_depthFunc();
  void set_depthFunc(const Nullable<int>& value);
  bool get_depthMask() const;
  void set_depthMask(bool value);
  bool get_depthTest() const;
  void set_depthTest(bool value);
  Nullable<unsigned int>& get_frontFace();
  void set_frontFace(const Nullable<unsigned int>& value);

public:
  ReadOnlyProperty<_DepthCullingState, bool> isDirty;
  Property<_DepthCullingState, float> zOffset;
  Property<_DepthCullingState, Nullable<int>> cullFace;
  Property<_DepthCullingState, Nullable<bool>> cull;
  Property<_DepthCullingState, Nullable<int>> depthFunc;
  Property<_DepthCullingState, bool> depthMask;
  Property<_DepthCullingState, bool> depthTest;
  Property<_DepthCullingState, Nullable<unsigned int>> frontFace;

private:
  bool _isDepthTestDirty;
  bool _isDepthMaskDirty;
  bool _isDepthFuncDirty;
  bool _isCullFaceDirty;
  bool _isCullDirty;
  bool _isZOffsetDirty;
  bool _isFrontFaceDirty;

  bool _depthTest;
  bool _depthMask;
  Nullable<int> _depthFunc;
  Nullable<bool> _cull;
  Nullable<int> _cullFace;
  float _zOffset;
  Nullable<unsigned int> _frontFace;

}; // end of class _DepthCullingState

} // end of namespace BABYLON

#endif // end of BABYLON_DEPTH_CULLING_STATE_H
