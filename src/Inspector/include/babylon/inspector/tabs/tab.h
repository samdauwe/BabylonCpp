#ifndef BABYLON_INSPECTOR_TABS_TAB_H
#define BABYLON_INSPECTOR_TABS_TAB_H

#include <babylon/babylon_global.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Tab {

public:
  Tab(const string_t& name);
  virtual ~Tab();

  virtual void render()  = 0;
  virtual void dispose() = 0;

public:
  // The tab name displayed in the tabbar
  string_t name;

}; // end of class Tab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_TAB_H
