#include <imgui.h>
#include <functional>

namespace ImGuiUtils
{
  namespace ImGuiRunner
  {
    using GuiFunction = std::function<void(void)>;

    void RunGui(GuiFunction guiFunction);
  }
}
