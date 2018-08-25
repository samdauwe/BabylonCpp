#ifndef BABYLON_DEPTH_CULLING_STATE_H
#define BABYLON_DEPTH_CULLING_STATE_H

#include <babylon/babylon_global.h>

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
  nullable_t<int>& get_cullFace();
  void set_cullFace(const nullable_t<int>& value);
  nullable_t<bool>& get_cull();
  void set_cull(const nullable_t<bool>& value);
  nullable_t<int>& get_depthFunc();
  void set_depthFunc(const nullable_t<int>& value);
  bool get_depthMask() const;
  void set_depthMask(bool value);
  bool get_depthTest() const;
  void set_depthTest(bool value);
  nullable_t<unsigned int>& get_frontFace();
  void set_frontFace(const nullable_t<unsigned int>& value);

public:
  ReadOnlyProperty<_DepthCullingState, bool> isDirty;
  Property<_DepthCullingState, float> zOffset;
  Property<_DepthCullingState, nullable_t<int>> cullFace;
  Property<_DepthCullingState, nullable_t<bool>> cull;
  Property<_DepthCullingState, nullable_t<int>> depthFunc;
  Property<_DepthCullingState, bool> depthMask;
  Property<_DepthCullingState, bool> depthTest;
  Property<_DepthCullingState, nullable_t<unsigned int>> frontFace;

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
  nullable_t<int> _depthFunc;
  nullable_t<bool> _cull;
  nullable_t<int> _cullFace;
  float _zOffset;
  nullable_t<unsigned int> _frontFace;

}; // end of class _DepthCullingState

} // end of namespace BABYLON

#endif // end of BABYLON_DEPTH_CULLING_STATE_H
