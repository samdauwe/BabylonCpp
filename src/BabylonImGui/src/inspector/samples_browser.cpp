#ifdef _MSC_VER
#include <filesystem>
#endif
#include <babylon/babylon_common.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/system.h>
#include <babylon/inspector/samples_browser.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/misc/string_tools.h>
#include <babylon/samples/samples_index.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>
#include <map>

#include <imgui.h>

#include <babylon/inspector/inspector.h>

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

const std::string screenshotsFolderCurrent = "screenshots/samples_current/";

} // end anonymous namespace

namespace BABYLON {

using namespace BABYLON::SamplesInfo;

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
    if (ImGui::InputText_String("Filter", _query.query))
      changed = true;

    ImGui::SameLine();
    if (ImGui::Checkbox(ICON_FA_WRENCH "Experimental", &_query.onlyFailures))
      changed = true;

    if (OnLoopSamples) {
      if (ImGui::Button("Loop filtered samples")) {
        std::vector<std::string> filteredSamples;
        for (const auto& kv : _matchingSamples) {
          for (auto sampleName : kv.second)
            filteredSamples.push_back(sampleName);
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
    for (const auto& kv : _matchingSamples) {
      CategoryName category = kv.first;
      auto samples          = kv.second;
      if (!samples.empty()) {
        std::string header = category + " (" + std::to_string(samples.size()) + ")";
        if (collapseMode == CollapseMode::CollapseAll)
          ImGui::SetNextItemOpen(false);
        if (collapseMode == CollapseMode::ExpandAll)
          ImGui::SetNextItemOpen(true);
        if (ImGui::CollapsingHeader(header.c_str())) // ImGuiTreeNodeFlags_DefaultOpen))
        {
          for (const std::string& sample : samples) {
            guiOneSample(sample);
            ImGui::Separator();
          }
        }
      }
    }
    ImGui::EndChild();
  }

  void guiOneSampleLinks(const std::string& sampleName)
  {
    const auto& sampleInfo = _samplesCollection.GetSampleByName(sampleName)->sourceInfo;

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

  void guiOneSampleInfos(const std::string& sampleName)
  {
    const auto& sampleInfo = _samplesCollection.GetSampleByName(sampleName)->sourceInfo;

    std::string runLabel = std::string(ICON_FA_PLAY_CIRCLE " Run##") + sampleName;
    if (ImGui::Button(runLabel.c_str())) {
      if (OnNewRenderableScene) {
        BABYLON::ICanvas* dummyCanvas = nullptr;
        auto scene = _samplesCollection.createRenderableScene(sampleName, dummyCanvas);
        OnNewRenderableScene(std::move(scene));
      }
      if (Inspector::OnSampleChanged)
        Inspector::OnSampleChanged(sampleName);
    }

    if (OnEditFiles) {
      ImGui::SameLine();
      std::string viewCodeLabel = ICON_FA_EDIT " View code##" + sampleName;
      if (ImGui::Button(viewCodeLabel.c_str())) {
        std::string sample_cpp_file
          = SamplesInfo::SamplesProjectFolder() + "/" + sampleInfo.SourceFile;
        OnEditFiles({sample_cpp_file});
      }
    }
  }

  void guiOneSample(const std::string& sampleName)
  {
    const auto& sampleInfo = _samplesCollection.GetSampleByName(sampleName)->sourceInfo;
    std::string currentScreenshotFile = screenshotsFolderCurrent + sampleName + ".jpg";
    std::string sample_snake          = to_snake_case(sampleName);

    if (_showCurrentScreenshots) {
      ImGui::BeginGroup();
      ImVec2 imageSize(ImGui::GetWindowWidth() / 3.f, 0.f);
      ImGuiUtils::ImageFromFile(currentScreenshotFile, imageSize);
      ImGui::EndGroup();
      ImGui::SameLine();
    }

    ImGui::BeginGroup();
    ImGui::Text("%s", sampleName.c_str());
    ImGui::TextWrapped("%s", sampleInfo.Brief.c_str());

//    bool failure = false;
//    if (failure) {
//      ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.3f, 1.f), "Failure: %s",
//                         SampleFailureReason_Str(failure.value().Kind).c_str());
//      if (!failure.value().Info.empty())
//        ImGui::TextColored(ImVec4(0.4f, 0.9f, 0.6f, 1.f), "More details: %s",
//                           failure.value().Info.c_str());
//    }
    guiOneSampleInfos(sampleName);
    ImGui::EndGroup();
    // ImGui::SameLine();

    guiOneSampleLinks(sampleName);
  }

  bool doesSampleMatchQuery(const CategoryName& categoryName, const SampleName& sampleName)
  {
    std::vector<std::string> search_items = BABYLON::StringTools::split(_query.query, ' ');

    bool doesMatch = true;

    {
      std::string all = BABYLON::StringTools::toLowerCase(categoryName + " / " + sampleName);
      for (const auto& item : search_items)
        if (!BABYLON::StringTools::contains(all, BABYLON::StringTools::toLowerCase(item)))
          doesMatch = false;
    }

//    if (_query.onlyFailures) {
//      if (!samplesCollection.doesSampleFail(sampleName))
//        doesMatch = false;
//    }
//    else {
//      if (samplesCollection.doesSampleFail(sampleName))
//        doesMatch = false;
//    }

    return doesMatch;
  }

  void fillMatchingSamples()
  {
    _matchingSamples.clear();
    for (const auto & sampleData: _samplesCollection.AllSamples())
      _matchingSamples[sampleData.categoryName].push_back(sampleData.sampleName);

//    for (CategoryName category : samplesCollection.getCategoryNames()) {
//      std::vector<SampleName> s;
//      for (SampleName sample : samplesCollection.getSampleNamesInCategory(category)) {
//        if (doesSampleMatchQuery(category, sample))
//          s.push_back(sample);
//        _matchingSamples[category] = s;
//      }
//    }
  }

  size_t nbMatchingSamples()
  {
    size_t r = 0;
    for (const auto& kv : _matchingSamples)
      r += kv.second.size();
    return r;
  }

  SamplesInfo::SamplesCollection& _samplesCollection;
  std::map<CategoryName, std::vector<SampleName>> _matchingSamples;

  struct {
    std::string query = "";
    bool onlyFailures = false;
  } _query;

  bool _showCurrentScreenshots = true;
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
