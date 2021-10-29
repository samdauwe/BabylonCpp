#ifndef BABYLONIMGUI_BABYLON_LOGS_POPUP_H
#define BABYLONIMGUI_BABYLON_LOGS_POPUP_H
#include <memory>

namespace ImGuiAl {
class Log;
} // end of namespace ImGuiAl

namespace BABYLON {

class BabylonLogsWindow {
public:
  static BabylonLogsWindow& instance();
  void render();

  ImGuiAl::Log* Impl();

private:
  BabylonLogsWindow();
  ~BabylonLogsWindow();

private:
  std::unique_ptr<ImGuiAl::Log> _logImpl;
};

} // end of namespace BABYLON

#endif // #ifndef BABYLONIMGUI_BABYLON_LOGS_POPUP_H
