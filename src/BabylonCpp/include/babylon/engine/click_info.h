#ifndef BABYLON_ENGINE_CLICK_INFO_H
#define BABYLON_ENGINE_CLICK_INFO_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT ClickInfo {

public:
  ClickInfo();
  ClickInfo(const ClickInfo& otherClickInfo);
  ClickInfo(ClickInfo&& otherClickInfo);
  ClickInfo& operator=(const ClickInfo& otherClickInfo);
  ClickInfo& operator=(ClickInfo&& otherClickInfo);
  ~ClickInfo();

  bool singleClick() const;
  bool doubleClick() const;
  bool hasSwiped() const;
  bool ignore() const;

  void setSingleClick(bool b);
  void setDoubleClick(bool b);
  void setHasSwiped(bool b);
  void setIgnore(bool b);

private:
  bool _singleClick;
  bool _doubleClick;
  bool _hasSwiped;
  bool _ignore;

}; // end of struct PointerInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_CLICK_INFO_H
