#include <babylon/inputs/click_info.h>

namespace BABYLON {

ClickInfo::ClickInfo()
    : singleClick{this, &ClickInfo::get_singleClick, &ClickInfo::set_singleClick}
    , doubleClick{this, &ClickInfo::get_doubleClick, &ClickInfo::set_doubleClick}
    , hasSwiped{this, &ClickInfo::get_hasSwiped, &ClickInfo::set_hasSwiped}
    , ignore{this, &ClickInfo::get_ignore, &ClickInfo::set_ignore}
    , _singleClick{false}
    , _doubleClick{false}
    , _hasSwiped{false}
    , _ignore{false}
{
}

ClickInfo::ClickInfo(const ClickInfo& otherClickInfo)
    : singleClick{this, &ClickInfo::get_singleClick, &ClickInfo::set_singleClick}
    , doubleClick{this, &ClickInfo::get_doubleClick, &ClickInfo::set_doubleClick}
    , hasSwiped{this, &ClickInfo::get_hasSwiped, &ClickInfo::set_hasSwiped}
    , ignore{this, &ClickInfo::get_ignore, &ClickInfo::set_ignore}
    , _singleClick{otherClickInfo._singleClick}
    , _doubleClick{otherClickInfo._doubleClick}
    , _hasSwiped{otherClickInfo._hasSwiped}
    , _ignore{otherClickInfo._ignore}
{
}

ClickInfo::ClickInfo(ClickInfo&& otherClickInfo)
    : singleClick{this, &ClickInfo::get_singleClick, &ClickInfo::set_singleClick}
    , doubleClick{this, &ClickInfo::get_doubleClick, &ClickInfo::set_doubleClick}
    , hasSwiped{this, &ClickInfo::get_hasSwiped, &ClickInfo::set_hasSwiped}
    , ignore{this, &ClickInfo::get_ignore, &ClickInfo::set_ignore}
    , _singleClick{std::move(otherClickInfo._singleClick)}
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

ClickInfo::~ClickInfo() = default;

bool ClickInfo::get_singleClick() const
{
  return _singleClick;
}

bool ClickInfo::get_doubleClick() const
{
  return _doubleClick;
}

bool ClickInfo::get_hasSwiped() const
{
  return _hasSwiped;
}

bool ClickInfo::get_ignore() const
{
  return _ignore;
}

void ClickInfo::set_singleClick(bool b)
{
  _singleClick = b;
}

void ClickInfo::set_doubleClick(bool b)
{
  _doubleClick = b;
}

void ClickInfo::set_hasSwiped(bool b)
{
  _hasSwiped = b;
}

void ClickInfo::set_ignore(bool b)
{
  _ignore = b;
}

} // end of namespace BABYLON
