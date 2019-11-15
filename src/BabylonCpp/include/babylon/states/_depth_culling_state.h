#ifndef BABYLON_DEPTH_CULLING_STATE_H
#define BABYLON_DEPTH_CULLING_STATE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

namespace GL {
class IGLRenderingContext;
} // end of namespace GL

class BABYLON_SHARED_EXPORT _DepthCullingState {

public:
  /**
   * @brief Initializes the state.
   */
  _DepthCullingState();
  ~_DepthCullingState(); // = default

  void reset();
  void apply(GL::IGLRenderingContext& _gl);

protected:
  bool get_isDirty() const;
  float get_zOffset() const;
  void set_zOffset(float value);
  std::optional<int>& get_cullFace();
  void set_cullFace(const std::optional<int>& value);
  std::optional<bool>& get_cull();
  void set_cull(const std::optional<bool>& value);
  std::optional<int>& get_depthFunc();
  void set_depthFunc(const std::optional<int>& value);
  bool get_depthMask() const;
  void set_depthMask(bool value);
  bool get_depthTest() const;
  void set_depthTest(bool value);
  std::optional<unsigned int>& get_frontFace();
  void set_frontFace(const std::optional<unsigned int>& value);

public:
  ReadOnlyProperty<_DepthCullingState, bool> isDirty;
  Property<_DepthCullingState, float> zOffset;
  Property<_DepthCullingState, std::optional<int>> cullFace;
  Property<_DepthCullingState, std::optional<bool>> cull;
  Property<_DepthCullingState, std::optional<int>> depthFunc;
  Property<_DepthCullingState, bool> depthMask;
  Property<_DepthCullingState, bool> depthTest;
  Property<_DepthCullingState, std::optional<unsigned int>> frontFace;

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
  std::optional<int> _depthFunc;
  std::optional<bool> _cull;
  std::optional<int> _cullFace;
  float _zOffset;
  std::optional<unsigned int> _frontFace;

}; // end of class _DepthCullingState

} // end of namespace BABYLON

#endif // end of BABYLON_DEPTH_CULLING_STATE_H
