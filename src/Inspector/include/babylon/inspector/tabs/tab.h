#ifndef BABYLON_INSPECTOR_TABS_TAB_H
#define BABYLON_INSPECTOR_TABS_TAB_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Tab {

public:
  Tab(const std::string& name);
  virtual ~Tab();

  virtual void render()  = 0;
  virtual void dispose() = 0;

public:
  // The tab name displayed in the tabbar
  std::string name;

}; // end of class Tab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_TAB_H
