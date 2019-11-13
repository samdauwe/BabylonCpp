#include <babylon/states/_depth_culling_state.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {

_DepthCullingState::_DepthCullingState()
    : isDirty{this, &_DepthCullingState::get_isDirty}
    , zOffset{this, &_DepthCullingState::get_zOffset,
              &_DepthCullingState::set_zOffset}
    , cullFace{this, &_DepthCullingState::get_cullFace,
               &_DepthCullingState::set_cullFace}
    , cull{this, &_DepthCullingState::get_cull, &_DepthCullingState::set_cull}
    , depthFunc{this, &_DepthCullingState::get_depthFunc,
                &_DepthCullingState::set_depthFunc}
    , depthMask{this, &_DepthCullingState::get_depthMask,
                &_DepthCullingState::set_depthMask}
    , depthTest{this, &_DepthCullingState::get_depthTest,
                &_DepthCullingState::set_depthTest}
    , frontFace{this, &_DepthCullingState::get_frontFace,
                &_DepthCullingState::set_frontFace}
    , _isDepthTestDirty{false}
    , _isDepthMaskDirty{false}
    , _isDepthFuncDirty{false}
    , _isCullFaceDirty{false}
    , _isCullDirty{false}
    , _isZOffsetDirty{false}
    , _isFrontFaceDirty{false}
    , _depthFunc{std::nullopt}
    , _cull{std::nullopt}
    , _cullFace{std::nullopt}
    , _zOffset{0.f}
    , _frontFace{std::nullopt}
{
  reset();
}

_DepthCullingState::~_DepthCullingState() = default;

bool _DepthCullingState::get_isDirty() const
{
  return _isDepthFuncDirty || _isDepthTestDirty || _isDepthMaskDirty
         || _isCullFaceDirty || _isCullDirty || _isZOffsetDirty
         || _isFrontFaceDirty;
}

float _DepthCullingState::get_zOffset() const
{
  return _zOffset;
}

void _DepthCullingState::set_zOffset(float value)
{
  if (stl_util::almost_equal(_zOffset, value)) {
    return;
  }

  _zOffset        = value;
  _isZOffsetDirty = true;
}

std::optional<int>& _DepthCullingState::get_cullFace()
{
  return _cullFace;
}

void _DepthCullingState::set_cullFace(const std::optional<int>& value)
{
  if (_cullFace == value) {
    return;
  }

  _cullFace        = value;
  _isCullFaceDirty = true;
}

std::optional<bool>& _DepthCullingState::get_cull()
{
  return _cull;
}

void _DepthCullingState::set_cull(const std::optional<bool>& value)
{
  if (_cull == value) {
    return;
  }

  _cull        = value;
  _isCullDirty = true;
}

std::optional<int>& _DepthCullingState::get_depthFunc()
{
  return _depthFunc;
}

void _DepthCullingState::set_depthFunc(const std::optional<int>& value)
{
  if (_depthFunc == value) {
    return;
  }

  _depthFunc        = value;
  _isDepthFuncDirty = true;
}

bool _DepthCullingState::get_depthMask() const
{
  return _depthMask;
}

void _DepthCullingState::set_depthMask(bool value)
{
  if (_depthMask == value) {
    return;
  }

  _depthMask        = value;
  _isDepthMaskDirty = true;
}

bool _DepthCullingState::get_depthTest() const
{
  return _depthTest;
}

void _DepthCullingState::set_depthTest(bool value)
{
  if (_depthTest == value) {
    return;
  }

  _depthTest        = value;
  _isDepthTestDirty = true;
}

std::optional<unsigned int>& _DepthCullingState::get_frontFace()
{
  return _frontFace;
}

void _DepthCullingState::set_frontFace(const std::optional<unsigned int>& value)
{
  if (_frontFace == value) {
    return;
  }

  _frontFace        = value;
  _isFrontFaceDirty = true;
}

void _DepthCullingState::reset()
{
  _depthMask = true;
  _depthTest = true;
  _depthFunc = std::nullopt;
  _cullFace  = std::nullopt;
  _cull      = std::nullopt;
  _zOffset   = 0.f;
  _frontFace = std::nullopt;

  _isDepthTestDirty = true;
  _isDepthMaskDirty = true;
  _isDepthFuncDirty = false;
  _isCullFaceDirty  = false;
  _isCullDirty      = false;
  _isZOffsetDirty   = false;
  _isFrontFaceDirty = false;
}

void _DepthCullingState::apply(GL::IGLRenderingContext& gl)
{
  if (!isDirty()) {
    return;
  }

  // Cull
  if (_isCullDirty) {
    const auto& iCullFace = cull();
    if (iCullFace && *iCullFace) {
      gl.enable(GL::CULL_FACE);
    }
    else {
      gl.disable(GL::CULL_FACE);
    }

    _isCullDirty = false;
  }

  // Cull face
  if (_isCullFaceDirty) {
    gl.cullFace(static_cast<unsigned int>(*cullFace()));
    _isCullFaceDirty = false;
  }

  // Depth mask
  if (_isDepthMaskDirty) {
    gl.depthMask(depthMask());
    _isDepthMaskDirty = false;
  }

  // Depth test
  if (_isDepthTestDirty) {
    if (depthTest()) {
      gl.enable(GL::DEPTH_TEST);
    }
    else {
      gl.disable(GL::DEPTH_TEST);
    }
    _isDepthTestDirty = false;
  }

  // Depth func
  if (_isDepthFuncDirty) {
    gl.depthFunc(static_cast<unsigned int>(*depthFunc()));
    _isDepthFuncDirty = false;
  }

  // zOffset
  if (_isZOffsetDirty) {
    if (!stl_util::almost_equal(zOffset(), 0.f)) {
      gl.enable(GL::POLYGON_OFFSET_FILL);
      gl.polygonOffset(zOffset(), 0);
    }
    else {
      gl.disable(GL::POLYGON_OFFSET_FILL);
    }

    _isZOffsetDirty = false;
  }

  // Front face
  if (_isFrontFaceDirty) {
    gl.frontFace(*frontFace());
    _isFrontFaceDirty = false;
  }
}

} // end of namespace BABYLON
