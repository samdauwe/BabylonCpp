#ifndef BABYLON_DEPTH_CULLING_STATE_H
#define BABYLON_DEPTH_CULLING_STATE_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

namespace GL {
class IGLRenderingContext;
} // end of namespace GL

class BABYLON_SHARED_EXPORT DepthCullingState {

public:
  /**
   * @brief Initializes the state.
   */
  DepthCullingState(bool reset = true);
  ~DepthCullingState(); // = default

  void reset();
  void apply(GL::IGLRenderingContext& _gl);

protected:
  bool get_isDirty() const;
  float get_zOffset() const;
  void set_zOffset(float value);
  float get_zOffsetUnits() const;
  void set_zOffsetUnits(float value);
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
  ReadOnlyProperty<DepthCullingState, bool> isDirty;
  Property<DepthCullingState, float> zOffset;
  Property<DepthCullingState, float> zOffsetUnits;
  Property<DepthCullingState, std::optional<int>> cullFace;
  Property<DepthCullingState, std::optional<bool>> cull;
  Property<DepthCullingState, std::optional<int>> depthFunc;
  Property<DepthCullingState, bool> depthMask;
  Property<DepthCullingState, bool> depthTest;
  Property<DepthCullingState, std::optional<unsigned int>> frontFace;

protected:
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
  float _zOffsetUnits;
  std::optional<unsigned int> _frontFace;

}; // end of class DepthCullingState

} // end of namespace BABYLON

#endif // end of BABYLON_DEPTH_CULLING_STATE_H
