#include <babylon/engine/click_info.h>

namespace BABYLON {

ClickInfo::ClickInfo()
    : _singleClick{false}
    , _doubleClick{false}
    , _hasSwiped{false}
    , _ignore{false}
{
}

ClickInfo::ClickInfo(const ClickInfo& otherClickInfo)
    : _singleClick{otherClickInfo._singleClick}
    , _doubleClick{otherClickInfo._doubleClick}
    , _hasSwiped{otherClickInfo._hasSwiped}
    , _ignore{otherClickInfo._ignore}
{
}

ClickInfo::ClickInfo(ClickInfo&& otherClickInfo)
    : _singleClick{std::move(otherClickInfo._singleClick)}
    , _doubleClick{std::move(otherClickInfo._doubleClick)}
    , _hasSwiped{std::move(otherClickInfo._hasSwiped)}
    , _ignore{std::move(otherClickInfo._ignore)}
{
}

ClickInfo& ClickInfo::operator=(const ClickInfo& otherClickInfo)
{
  if (&otherClickInfo != this) {
    _singleClick = otherClickInfo._singleClick;
    _doubleClick = otherClickInfo._doubleClick;
    _hasSwiped   = otherClickInfo._hasSwiped;
    _ignore      = otherClickInfo._ignore;
  }

  return *this;
}

ClickInfo& ClickInfo::operator=(ClickInfo&& otherClickInfo)
{
  if (&otherClickInfo != this) {
    _singleClick = std::move(otherClickInfo._singleClick);
    _doubleClick = std::move(otherClickInfo._doubleClick);
    _hasSwiped   = std::move(otherClickInfo._hasSwiped);
    _ignore      = std::move(otherClickInfo._ignore);
  }

  return *this;
}

ClickInfo::~ClickInfo()
{
}

bool ClickInfo::singleClick() const
{
  return _singleClick;
}

bool ClickInfo::doubleClick() const
{
  return _doubleClick;
}

bool ClickInfo::hasSwiped() const
{
  return _hasSwiped;
}

bool ClickInfo::ignore() const
{
  return _ignore;
}

void ClickInfo::setSingleClick(bool b)
{
  _singleClick = b;
}

void ClickInfo::setDoubleClick(bool b)
{
  _doubleClick = b;
}

void ClickInfo::setHasSwiped(bool b)
{
  _hasSwiped = b;
}

void ClickInfo::setIgnore(bool b)
{
  _ignore = b;
}

} // end of namespace BABYLON
