#include <babylon/behaviors/meshes/fade_in_out_behavior.h>

#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

FadeInOutBehavior::FadeInOutBehavior()
    : delay{0.f}
    , fadeInTime{300.f}
    , _millisecondsPerFrame{1000.f / 60.f}
    , _hovered{false}
    , _ownerNode{nullptr}
{
}

FadeInOutBehavior::~FadeInOutBehavior() = default;

const char* FadeInOutBehavior::name() const
{
  return "FadeInOut";
}

void FadeInOutBehavior::init()
{
}

void FadeInOutBehavior::attach(
  const MeshPtr& ownerNode,
  const std::function<bool(const AbstractMeshPtr& m)>& /*predicate*/)
{
  _ownerNode = ownerNode;
  _setAllVisibility(_ownerNode, 0);
}

void FadeInOutBehavior::detach()
{
  _ownerNode = nullptr;
}

void FadeInOutBehavior::fadeIn(bool value)
{
  _hovered = value;
  _update();
}

void FadeInOutBehavior::_update()
{
  if (_ownerNode) {
    _hoverValue += _hovered ? _millisecondsPerFrame : -_millisecondsPerFrame;

    _setAllVisibility(_ownerNode, (_hoverValue - delay) / fadeInTime);

    if (_ownerNode->visibility > 1.f) {
      _setAllVisibility(_ownerNode, 1.f);
      _hoverValue = fadeInTime + delay;
      return;
    }
    else if (_ownerNode->visibility < 0.f) {
      _setAllVisibility(_ownerNode, 0.f);
      if (_hoverValue < 0.f) {
        _hoverValue = 0.f;
        return;
      }
    }
  }
}

void FadeInOutBehavior::_setAllVisibility(const AbstractMeshPtr& mesh,
                                          float value)
{
  mesh->visibility = value;
  for (const auto& c : mesh->getChildMeshes()) {
    _setAllVisibility(c, value);
  }
}

} // end of namespace BABYLON
