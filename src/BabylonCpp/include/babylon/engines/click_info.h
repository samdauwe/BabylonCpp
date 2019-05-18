#ifndef BABYLON_ENGINES_CLICK_INFO_H
#define BABYLON_ENGINES_CLICK_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT ClickInfo {

public:
  ClickInfo();
  ClickInfo(const ClickInfo& otherClickInfo);
  ClickInfo(ClickInfo&& otherClickInfo);
  ClickInfo& operator=(const ClickInfo& otherClickInfo);
  ClickInfo& operator=(ClickInfo&& otherClickInfo);
  ~ClickInfo();

private:
  bool get_singleClick() const;
  bool get_doubleClick() const;
  bool get_hasSwiped() const;
  bool get_ignore() const;

  void set_singleClick(bool b);
  void set_doubleClick(bool b);
  void set_hasSwiped(bool b);
  void set_ignore(bool b);

public:
  Property<ClickInfo, bool> singleClick;
  Property<ClickInfo, bool> doubleClick;
  Property<ClickInfo, bool> hasSwiped;
  Property<ClickInfo, bool> ignore;

private:
  bool _singleClick;
  bool _doubleClick;
  bool _hasSwiped;
  bool _ignore;

}; // end of class PointerInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_CLICK_INFO_H
