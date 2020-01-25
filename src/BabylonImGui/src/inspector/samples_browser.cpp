#ifdef _MSC_VER
#include <filesystem>
#endif
#include <babylon/babylon_common.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/system.h>
#include <babylon/inspector/samples_browser.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/misc/string_tools.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>
#include <map>
#include <magic_enum.hpp>

#include <imgui.h>

#include <babylon/inspector/inspector.h>

using namespace BABYLON::SamplesInfo;

namespace {
std::string to_snake_case(const std::string& sPascalCase)
{
  std::stringstream ss;
  bool first = true;
  for (auto c : sPascalCase) {
    if (std::tolower(c) != c) {
      if (!first)
        ss << "_";
      ss << static_cast<unsigned char>(std::tolower(c));
    }
    else
      ss << c;
    first = false;
  }
  return ss.str();
}

} // end anonymous namespace

namespace BABYLON {


class SamplesBrowserImpl {
public:
  using CategoryName = std::string;
  using SampleName   = std::string;

  SamplesBrowserImpl() : _samplesCollection(SamplesCollection::Instance())
  {
    fillMatchingSamples();
  }

  void render()
  {
    render_filter();
    ImGui::Separator();
    render_list();
  }

  SamplesBrowser::CallbackNewRenderableScene OnNewRenderableScene;
  SamplesBrowser::CallbackEditFiles OnEditFiles;
  SamplesBrowser::CallbackLoopSamples OnLoopSamples;

private:
  void render_filter()
  {
    bool changed = false;
    ImGui::PushItemWidth(200);
    if (ImGui::InputText_String("Search " ICON_FA_SEARCH, _sampleSearchQuery.Query))
      changed = true;
    ImGui::SameLine();
    ImGui::Checkbox(ICON_FA_WRENCH "Experimental", &_queryExperimental);

    if (_queryExperimental)
    {
      for (auto status: magic_enum::enum_values<SamplesInfo::SampleAutoRunStatus>())
      {
       bool v = this->_sampleSearchQuery.IncludeStatus[status];
        std::string statusName(magic_enum::enum_name(status));
        if (ImGui::Checkbox(statusName.c_str(), &v))
        {
          this->_sampleSearchQuery.IncludeStatus[status] = v;
          changed = true;
        }
      }
      ImGui::Separator();

      if (ImGui::Checkbox("With manual run failures", &_sampleSearchQuery.OnlyManualRunFailure))
        changed = true;
    }

    if (OnLoopSamples) {
      if (ImGui::Button("Loop filtered samples")) {
        std::vector<std::string> filteredSamples;
        for (const auto& [categoryName, samples] : _matchingSamples) {
          for (const auto & sample: samples)
            filteredSamples.push_back(sample->sampleName);
        }
        OnLoopSamples(filteredSamples);
      }
    }

    if (changed)
      fillMatchingSamples();

    ImGui::Text("Matching samples : %zi", nbMatchingSamples());
  }

  void render_list()
  {
    enum class CollapseMode { None, CollapseAll, ExpandAll };
    CollapseMode collapseMode = CollapseMode::None;
    if (ImGui::Button("Collapse All"))
      collapseMode = CollapseMode::CollapseAll;
    ImGui::SameLine();
    if (ImGui::Button("Expand All"))
      collapseMode = CollapseMode::ExpandAll;

    ImGui::Separator();

    ImGui::BeginChild("Child1");
    for (const auto& [categoryName, samples] : _matchingSamples)
    {
      if (!samples.empty())
      {
        std::string header = categoryName + " (" + std::to_string(samples.size()) + ")";
        if (collapseMode == CollapseMode::CollapseAll)
          ImGui::SetNextItemOpen(false);
        if (collapseMode == CollapseMode::ExpandAll)
          ImGui::SetNextItemOpen(true);
        if (ImGui::CollapsingHeader(header.c_str())) // ImGuiTreeNodeFlags_DefaultOpen))
        {
          for (const SampleData* sample : samples)
          {
            guiOneSample(*sample);
            ImGui::Separator();
          }
        }
      }
    }
    ImGui::EndChild();
  }

  void guiOneSampleLinks(const SampleData& sampleData)
  {
    const auto& sampleInfo = sampleData.sourceInfo;

    if (!sampleInfo.Links.empty()) {
      for (auto link : sampleInfo.Links) {
        std::string btnUrlString = std::string(ICON_FA_EXTERNAL_LINK_ALT "##") + link;
        if (ImGui::Button(btnUrlString.c_str()))
          BABYLON::System::openBrowser(link);
        ImGui::SameLine();
        ImVec4 linkColor(0.5f, 0.5f, 0.95f, 1.f);
        // float wrap_width = 350.f;
        ImGui::PushTextWrapPos(ImGui::GetWindowWidth() - 15.f);
        ImGui::TextColored(linkColor, "%s", link.c_str());
        ImGui::PopTextWrapPos();
      }
    }
  }

  void guiOneSampleInfos(const SamplesInfo::SampleData& sampleData)
  {
    const auto& sampleInfo = sampleData.sourceInfo;

    std::string runLabel = std::string(ICON_FA_PLAY_CIRCLE " Run##") + sampleData.sampleName;
    if (ImGui::Button(runLabel.c_str())) {
      if (OnNewRenderableScene) {
        BABYLON::ICanvas* dummyCanvas = nullptr;
        auto scene = _samplesCollection.createRenderableScene(sampleData.sampleName, dummyCanvas);
        OnNewRenderableScene(std::move(scene));
      }
      if (Inspector::OnSampleChanged)
        Inspector::OnSampleChanged(sampleData.sampleName);
    }

    if (OnEditFiles && !sampleInfo.SourceFile.empty()) {
      ImGui::SameLine();
      std::string viewCodeLabel = ICON_FA_EDIT " View code##" + sampleData.sampleName;
      if (ImGui::Button(viewCodeLabel.c_str())) {
        std::string sample_cpp_file
          = SamplesInfo::SamplesProjectFolder() + "/" + sampleInfo.SourceFile;
        OnEditFiles({sample_cpp_file});
      }
    }
  }

  void guiOneSample(const SamplesInfo::SampleData& sampleData)
  {
    const auto& sampleInfo =  sampleData.sourceInfo;
    std::string currentScreenshotFile = screenshotsDirectory() + "/" + sampleData.sampleName + ".jpg";
    std::string sample_snake          = to_snake_case(sampleData.sampleName);

    // Screenshot
    {
      ImGui::BeginGroup();
      ImVec2 imageSize(ImGui::GetWindowWidth() / 3.f, 0.f);
      ImGuiUtils::ImageFromFile(currentScreenshotFile, imageSize);
      ImGui::EndGroup();
      ImGui::SameLine();
    }

    ImGui::BeginGroup();
    ImGui::Text("%s", sampleData.sampleName.c_str());
    ImGui::TextWrapped("%s", sampleInfo.Brief.c_str());

    if (sampleData.autoRunInfo.sampleRunStatus != SamplesInfo::SampleAutoRunStatus::success)
    {
      std::string_view status = magic_enum::enum_name(sampleData.autoRunInfo.sampleRunStatus);
      std::string statusStr(status);
      ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.3f, 1.f), "Failure: %s",
                         statusStr.c_str());
    }

    if (_queryExperimental)
    {
      ImGui::Separator();
      ImGui::Text("Manual run details");
      auto sampleManualRunInfoCopy = sampleData.sampleManualRunInfo;
      std::string checkboxLabel = std::string("ManualRun Fail##") + sampleData.sampleName;
      if (ImGui::Checkbox(checkboxLabel.c_str(), &sampleManualRunInfoCopy.failing))
        this->_samplesCollection.SetSampleManualRunInfo(sampleData.sampleName, sampleManualRunInfoCopy);

      char detail[1000];
      strncpy(detail, sampleManualRunInfoCopy.detail.c_str(), 1000);
      if (ImGui::InputTextMultiline(
        (std::string("##Detail") + sampleData.sampleName).c_str(),
        detail,
        1000,
        ImVec2(230.f,40.f)
        )
        )
      {
        sampleManualRunInfoCopy.detail = detail;
        this->_samplesCollection.SetSampleManualRunInfo(sampleData.sampleName, sampleManualRunInfoCopy);
      }
      ImGui::Separator();
    }

    guiOneSampleInfos(sampleData);
    ImGui::EndGroup();
    // ImGui::SameLine();

    guiOneSampleLinks(sampleData);
  }


  void fillMatchingSamples()
  {
    _matchingSamples = _samplesCollection.SearchSamples(_sampleSearchQuery);
  }

  size_t nbMatchingSamples()
  {
    size_t r = 0;
    for (const auto& kv : _matchingSamples)
      r += kv.second.size();
    return r;
  }

  SamplesInfo::SamplesCollection& _samplesCollection;
  std::map<SamplesInfo::CategoryName, std::vector<const SampleData *>> _matchingSamples;

  SamplesInfo::SampleSearchQuery _sampleSearchQuery;
  bool _queryExperimental = false;
};

SamplesBrowser::SamplesBrowser()
{
  pImpl = std::make_unique<SamplesBrowserImpl>();
}

SamplesBrowser::~SamplesBrowser() = default;

void SamplesBrowser::render()
{
  pImpl->OnNewRenderableScene = OnNewRenderableScene;
  pImpl->OnEditFiles          = OnEditFiles;
  pImpl->OnLoopSamples        = OnLoopSamples;
  pImpl->render();
}

} // end of namespace BABYLON
