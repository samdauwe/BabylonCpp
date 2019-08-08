#include <filesystem>
#include <babylon/core/string.h>
#include <babylon/imgui/icons_font_awesome_5.h>
#include <babylon/inspector/components/actiontabs/tabs/sample_list_component.h>

#include <imgui.h>

#include <babylon/inspector/inspector.h>
#include <babylon/samples/samples_index.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

namespace BABYLON {

namespace
{
  void openBrowser(const std::string &url)
  {
#ifndef _WIN32
    std::string cmd = std::string("open ") + url;
    system(cmd.c_str());
#else
    ShellExecute(0, 0, url.c_str(), 0, 0, SW_SHOW);
#endif
  }
  void openFile(const std::string &filename)
  {
    std::string canonical_path = std::filesystem::canonical(std::filesystem::path(filename)).string();
    std::string cmd = std::string("open ") + canonical_path;
#ifndef _WIN32
    system(cmd.c_str());
#else
    ShellExecute(0, 0, canonical_path.c_str(), 0, 0, SW_SHOW);
#endif
  }
}

SampleListComponent::SampleListComponent()
{
}

void GuiOneSample(const std::string &sampleName)
{
  bool canLaunch = Inspector::OnSampleChanged ?  true : false;

  static auto samples = Samples::SamplesIndex();
  auto sampleInfo = samples.getSampleInfo(sampleName);
  ImGui::TextColored(ImVec4(0.7f, 0.9f, 0.7f, 1.f), "%s", sampleName.c_str());
  ImGui::TextWrapped("%s", sampleInfo.Brief.c_str());

  if (canLaunch) {
    if (ImGui::Button(ICON_FA_PLAY_CIRCLE " Run"))
      Inspector::OnSampleChanged(sampleName);
  }

  std::string btnHeaderString = std::string(ICON_FA_EYE "##") + sampleInfo.HeaderFile;
  if (ImGui::Button(btnHeaderString.c_str()))
    openFile(sampleInfo.HeaderFile);
  ImGui::SameLine();
  ImGui::TextDisabled(".h  : %s", sampleInfo.HeaderFile.c_str());

  std::string btnSourceString = std::string(ICON_FA_EYE "##") + sampleInfo.SourceFile;
  if (ImGui::Button(btnSourceString.c_str()))
    openFile(sampleInfo.SourceFile);
  ImGui::SameLine();
  ImGui::TextDisabled(".cpp: %s", sampleInfo.SourceFile.c_str());

  if ( ! sampleInfo.Links.empty()) {
    for(auto link: sampleInfo.Links) {
      std::string btnUrlString = std::string(ICON_FA_EXTERNAL_LINK_ALT "##") + link;
      if (ImGui::Button(btnUrlString.c_str()))
        openBrowser(link);
      ImGui::SameLine();
      ImVec4 linkColor(0.5f, 0.5f, 0.95f, 1.f);
      ImGui::TextColored(linkColor, "%s", link.c_str());
    }
  }

}

bool ImGuiListBox_Helper(
    const std::string & label,
    int * current_item,
    const std::vector<std::string> & items,
    int height_in_items
    )
{
  std::vector<const char *> as_char_ptr;
  for (const auto & s : items)
    as_char_ptr.push_back(s.c_str());
  return ImGui::ListBox(label.c_str(), current_item, as_char_ptr.data(), (int)as_char_ptr.size(), height_in_items);
}

void ShowSearchSamples() {
  static auto samples = BABYLON::Samples::SamplesIndex();
  static char search[200];
  bool searchEdited = ImGui::InputText("Filter" , search, 200);

  std::vector<std::string> search_items = BABYLON::String::split(search, ' ');

  auto do_match_search_items = [&](const std::string & category, const std::string & name) {
    std::string all = BABYLON::String::toLowerCase(category + " / " + name);
    for (const auto &item : search_items)
      if (! BABYLON::String::contains(all, BABYLON::String::toLowerCase(item)))
        return false;
    return true;
  };

  std::vector<std::string> matchingSamples;
  {
    auto categoryNames = samples.getCategoryNames();
    for (const auto& categoryName : categoryNames) {
      auto sampleNames = samples.getSampleNamesInCategory(categoryName);
      for (const auto& sampleName : sampleNames) {
        const bool enabled = samples.isSampleEnabled(sampleName);
        if (enabled) {
          if (do_match_search_items(categoryName, sampleName)) {
            matchingSamples.push_back(categoryName + "/" + sampleName);
          }
        }
      }
    }
  }

  static int currentIndex = -1;
  if (searchEdited)
    currentIndex = -1;
  ImGui::PushItemWidth(400);
  ImGuiListBox_Helper("", & currentIndex, matchingSamples, 4);
  ImGui::Text("%i samples", (int)matchingSamples.size());
  if ((currentIndex >=0) && (currentIndex < (int)matchingSamples.size()))
  {
    ImGui::Separator();
    auto sampleCatAndName = matchingSamples[currentIndex];
    auto sampleName = BABYLON::String::split(sampleCatAndName, '/')[1];
    GuiOneSample(sampleName);
  }
}

void SampleListComponent::render()
{
  ShowSearchSamples();
}

} // end of namespace BABYLON
