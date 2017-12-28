#include <babylon/states/_depth_culling_state.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/igl_rendering_context.h>

namespace BABYLON {
namespace Internals {

_DepthCullingState::_DepthCullingState()
    : _isDepthTestDirty{false}
    , _isDepthMaskDirty{false}
    , _isDepthFuncDirty{false}
    , _isCullFaceDirty{false}
    , _isCullDirty{false}
    , _isZOffsetDirty{false}
    , _depthFunc{nullptr}
    , _cull{nullptr}
    , _cullFace{nullptr}
{
  reset();
}

_DepthCullingState::~_DepthCullingState()
{
}

bool _DepthCullingState::isDirty() const
{
  return _isDepthFuncDirty || _isDepthTestDirty || _isDepthMaskDirty
         || _isCullFaceDirty || _isCullDirty || _isZOffsetDirty;
}

float _DepthCullingState::zOffset() const
{
  return _zOffset;
}

void _DepthCullingState::setZOffset(float value)
{
  if (stl_util::almost_equal(_zOffset, value)) {
    return;
  }

  _zOffset        = value;
  _isZOffsetDirty = true;
}

Nullable<int> _DepthCullingState::cullFace() const
{
  return _cullFace;
}

void _DepthCullingState::setCullFace(const Nullable<int>& value)
{
  if (_cullFace == value) {
    return;
  }

  _cullFace        = value;
  _isCullFaceDirty = true;
}

Nullable<bool> _DepthCullingState::cull() const
{
  return _cull;
}

void _DepthCullingState::setCull(const Nullable<bool>& value)
{
  if (_cull == value) {
    return;
  }

  _cull        = value;
  _isCullDirty = true;
}

Nullable<int> _DepthCullingState::depthFunc() const
{
  return _depthFunc;
}

void _DepthCullingState::setDepthFunc(const Nullable<int>& value)
{
  if (_depthFunc == value) {
    return;
  }

  _depthFunc        = value;
  _isDepthFuncDirty = true;
}

bool _DepthCullingState::depthMask() const
{
  return _depthMask;
}

void _DepthCullingState::setDepthMask(bool value)
{
  if (_depthMask == value) {
    return;
  }

  _depthMask        = value;
  _isDepthMaskDirty = true;
}

bool _DepthCullingState::depthTest() const
{
  return _depthTest;
}

void _DepthCullingState::setDepthTest(bool value)
{
  if (_depthTest == value) {
    return;
  }

  _depthTest        = value;
  _isDepthTestDirty = true;
}

void _DepthCullingState::reset()
{
  _depthMask = true;
  _depthTest = true;
  _depthFunc = nullptr;
  _cullFace  = nullptr;
  _cull      = nullptr;
  _zOffset   = 0.f;

  _isDepthTestDirty = true;
  _isDepthMaskDirty = true;
  _isDepthFuncDirty = false;
  _isCullFaceDirty  = false;
  _isCullDirty      = false;
  _isZOffsetDirty   = false;
}

void _DepthCullingState::apply(GL::IGLRenderingContext& gl)
{
  if (!isDirty()) {
    return;
  }

  // Cull
  if (_isCullDirty) {
    if (cull()) {
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
}

} // end of namespace Internals
} // end of namespace BABYLON
