#include <babylon/babylon_imgui/babylon_logs_window.h>
#include <imgui_utils/imguial_log.h>
#include <babylon/core/logging.h>
#include <imgui_utils/icons_font_awesome_5.h>


namespace BABYLON
{

  auto logDelegate = SA::delegate<void(const LogMessage&)>{
    [](const BABYLON::LogMessage& logMessage) {
      ImGuiAl::Log::Level levelAl = ImGuiAl::Log::kDebug;
      if (logMessage.level() == LogLevels::LEVEL_INFO)
        levelAl = ImGuiAl::Log::kInfo;
      if (logMessage.level() == LogLevels::LEVEL_TRACE)
        levelAl = ImGuiAl::Log::kDebug;
      if (logMessage.level() == LogLevels::LEVEL_WARN)
        levelAl = ImGuiAl::Log::kWarn;
      if (logMessage.level() == LogLevels::LEVEL_ERROR)
        levelAl = ImGuiAl::Log::kError;
      BabylonLogsWindow::instance().Impl()->Message(logMessage.toString().c_str(), levelAl);
    }};

  BabylonLogsWindow::BabylonLogsWindow()
  {
    LoggerInstance().registerLogMessageListener(LogLevels::LEVEL_DEBUG, logDelegate);
    LoggerInstance().registerLogMessageListener(LogLevels::LEVEL_ERROR, logDelegate);
    LoggerInstance().registerLogMessageListener(LogLevels::LEVEL_INFO, logDelegate);
    LoggerInstance().registerLogMessageListener(LogLevels::LEVEL_TRACE, logDelegate);
    LoggerInstance().registerLogMessageListener(LogLevels::LEVEL_WARN, logDelegate);

    _logImpl = std::make_unique<ImGuiAl::Log>();
    // Initialize
    static const char* actions[] =
    {
      ICON_FA_COPY " Copy",
      ICON_FA_RECYCLE " Clear",
      NULL
    };

    _logImpl->Init(ImGuiAl::Log::kShowFilters, actions);
    _logImpl->SetLabel(ImGuiAl::Log::kDebug, ICON_FA_BUG " Debug");
    _logImpl->SetLabel(ImGuiAl::Log::kInfo, ICON_FA_INFO " Info");
    _logImpl->SetLabel(ImGuiAl::Log::kWarn, ICON_FA_EXCLAMATION_TRIANGLE " Warn");
    _logImpl->SetLabel(ImGuiAl::Log::kError, ICON_FA_WRENCH " Error");
    _logImpl->SetCumulativeLabel(" Cumulative");
    _logImpl->SetFilterHeaderLabel(ICON_FA_FILTER " Filters");
    _logImpl->SetFilterLabel(ICON_FA_SEARCH " Filter (inc,-exc)");

  }

  BabylonLogsWindow::~BabylonLogsWindow() = default;

  BABYLON::BabylonLogsWindow& BabylonLogsWindow::instance()
  {
    static BabylonLogsWindow instance;
    return instance;
  }

  void BabylonLogsWindow::render()
  {
    bool popStyle = false;
    if (_visible)
    {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.6f, 1.f));
      popStyle = true;
    }

    if (ImGui::Button(ICON_FA_INFO " Logs"))
      _visible = !_visible;

    if (_visible)
    {
      ImGui::SetNextWindowSize(ImVec2(800, 300), ImGuiCond_FirstUseEver);
      ImGui::SetNextWindowPos(ImVec2(30, 60), ImGuiCond_FirstUseEver);
      ImGui::Begin("Logs", &_visible);
      _logImpl->Draw();
      ImGui::End();
    }
    if (popStyle)
      ImGui::PopStyleColor();
  }

  void BabylonLogsWindow::setVisible(bool visible)
  {
    _visible = visible;
  }

  ImGuiAl::Log * BabylonLogsWindow::Impl()
  {
    return _logImpl.get();
  }

}