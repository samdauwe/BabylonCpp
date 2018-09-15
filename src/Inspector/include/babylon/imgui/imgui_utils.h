#pragma once

#include <cmath>
#include <imgui.h>
#include <imgui_internal.h>

// from https://github.com/juliettef/IconFontCppHeaders
#include <babylon/imgui/icons_font_awesome.h>

namespace ImGui {

/**
 * @brief Returns color "color-bot" #5db0d7 -> rgba(93, 176, 215, 1)
 * @return color "color-bot"
 */
inline ImVec4 GetColorBot()
{
  return ImVec4(0.36f, 0.69f, 0.84f, 1.0f);
}

/**
 * @brief Returns color "color-top" #f29766 -> rgba(242, 151, 102, 1)
 * @return color "color-bot"
 */
inline ImVec4 GetColorTop()
{
  return ImVec4(0.95f, 0.59f, 0.4f, 1.0f);
}

inline void SetupImGuiStyle(bool bStyleDark_, float alpha_)
{
  ImGuiStyle& style = ImGui::GetStyle();

  // light style from Pacôme Danhiez (user itamago)
  // https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
  style.Alpha                             = 1.0f;
  style.FrameRounding                     = 3.0f;
  style.Colors[ImGuiCol_Text]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled]     = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
  style.Colors[ImGuiCol_ChildWindowBg]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_PopupBg]          = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
  style.Colors[ImGuiCol_Border]           = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
  style.Colors[ImGuiCol_BorderShadow]     = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  style.Colors[ImGuiCol_FrameBg]          = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
  style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_TitleBg]          = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
  style.Colors[ImGuiCol_TitleBgActive]    = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg]        = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg]      = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab]    = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered]
    = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive]
    = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
  /*style.Colors[ImGuiCol_ComboBg]       = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);*/
  style.Colors[ImGuiCol_CheckMark]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab]        = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
  style.Colors[ImGuiCol_SliderGrabActive]  = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Button]            = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_ButtonHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive]      = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Header]            = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  style.Colors[ImGuiCol_HeaderHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  style.Colors[ImGuiCol_HeaderActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Column]            = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_ColumnHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  style.Colors[ImGuiCol_ColumnActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip]        = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive]  = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
#if 0
  style.Colors[ImGuiCol_CloseButton]       = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
  style.Colors[ImGuiCol_CloseButtonHovered]
    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
#endif
  style.Colors[ImGuiCol_PlotLines]        = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram]    = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered]
    = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  style.Colors[ImGuiCol_ModalWindowDarkening]
    = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

  if (bStyleDark_) {
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      float H, S, V;
      ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

      if (S < 0.1f) {
        V = 1.0f - V;
      }
      ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
      if (col.w < 1.00f) {
        col.w *= alpha_;
      }
    }
  }
  else {
    for (int i = 0; i <= ImGuiCol_COUNT; i++) {
      ImVec4& col = style.Colors[i];
      if (col.w < 1.00f) {
        col.x *= alpha_;
        col.y *= alpha_;
        col.z *= alpha_;
        col.w *= alpha_;
      }
    }
  }
}

/**
 * @brief Splitter implementation using SplitterBehavior in imgui_internal.h as
 * a helper.
 * Ref: https://github.com/ocornut/imgui/issues/319
 */
inline bool Splitter(bool split_vertically, float thickness, float* size1,
                     float* size2, float min_size1, float min_size2,
                     float splitter_long_axis_size = -1.0f)
{
  using namespace ImGui;
  ImGuiContext& g     = *GImGui;
  ImGuiWindow* window = g.CurrentWindow;
  ImGuiID id          = window->GetID("##Splitter");
  ImRect bb;
  auto bbMin = (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
  bb.Min.x   = window->DC.CursorPos.x + bbMin.x;
  bb.Min.y   = window->DC.CursorPos.y + bbMin.y;
  auto bbMax = CalcItemSize(split_vertically ?
                              ImVec2(thickness, splitter_long_axis_size) :
                              ImVec2(splitter_long_axis_size, thickness),
                            0.0f, 0.0f);
  bb.Max.x   = bb.Min.x + bbMax.x;
  bb.Max.y   = bb.Min.y + bbMax.y;
#ifdef __unix__
  return SplitterBehavior(id, bb, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y,
                          size1, size2, min_size1, min_size2, 0.0f);
#elif _WIN32
  return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y,
                          size1, size2, min_size1, min_size2, 0.0f);
#endif
}

inline bool CheckBoxFont(const char* name_, bool* pB_, const char* pOn_ = "[X]",
                         const char* pOff_ = "[  ]")
{
  if (*pB_) {
    ImGui::Text("%s", pOn_);
  }
  else {
    ImGui::Text("%s", pOff_);
  }
  bool bHover = false;
  bHover      = bHover || ImGui::IsItemHovered();
  ImGui::SameLine();
  ImGui::Text("%s", name_);
  bHover = bHover || ImGui::IsItemHovered();
  if (bHover && ImGui::IsMouseClicked(0)) {
    *pB_ = !*pB_;
    return true;
  }
  return false;
}

inline bool CheckBoxTick(const char* name_, bool* pB_)
{
  return CheckBoxFont(name_, pB_, ICON_FA_CHECK_SQUARE_O, ICON_FA_SQUARE_O);
}

inline bool MenuItemCheckbox(const char* name_, bool* pB_)
{
  bool retval = ImGui::MenuItem(name_);
  ImGui::SameLine();
  if (*pB_) {
    ImGui::Text(ICON_FA_CHECK_SQUARE_O);
  }
  else {
    ImGui::Text(ICON_FA_SQUARE_O);
  }
  if (retval) {
    *pB_ = !*pB_;
  }
  return retval;
}

struct FrameTimeHistogram {
  // configuration params - modify these at will
  static const int NUM = 101; // last value is from T-1 to inf.

  float dT      = 0.001f;       // in seconds, default 1ms
  float refresh = 1.0f / 60.0f; // set this to your target refresh rate

  static const int NUM_MARKERS = 2;
  float markers[NUM_MARKERS]   = {0.99f, 0.999f};

  // data
  ImVec2 size  = ImVec2(3.0f * NUM, 40.0f);
  float lastdT = 0.0f;
  float timesTotal;
  float countsTotal;
  float times[NUM];
  float counts[NUM];
  float hitchTimes[NUM];
  float hitchCounts[NUM];

  FrameTimeHistogram()
  {
    Clear();
  }

  void Clear()
  {
    timesTotal  = 0.0f;
    countsTotal = 0.0f;
    memset(times, 0, sizeof(times));
    memset(counts, 0, sizeof(counts));
    memset(hitchTimes, 0, sizeof(hitchTimes));
    memset(hitchCounts, 0, sizeof(hitchCounts));
  }

  int GetBin(float time_)
  {
    int bin = static_cast<int>(std::floor(time_ / dT));
    if (bin >= NUM) {
      bin = NUM - 1;
    }
    return bin;
  }

  void Update(float deltaT_)
  {
    if (deltaT_ < 0.0f) {
      assert(false);
      return;
    }
    int bin = GetBin(deltaT_);
    times[bin] += deltaT_;
    timesTotal += deltaT_;
    counts[bin] += 1.0f;
    countsTotal += 1.0f;

    float hitch  = std::fabs(lastdT - deltaT_);
    int deltaBin = GetBin(hitch);
    hitchTimes[deltaBin] += hitch;
    hitchCounts[deltaBin] += 1.0f;
    lastdT = deltaT_;
  }

  void PlotRefreshLines(float total_ = 0.0f, float* pValues_ = nullptr)
  {
    ImDrawList* draw        = ImGui::GetWindowDrawList();
    const ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 pad              = style.FramePadding;
    ImVec2 min              = ImGui::GetItemRectMin();
    min.x += pad.x;
    ImVec2 max = ImGui::GetItemRectMax();
    max.x -= pad.x;

    float xRefresh = (max.x - min.x) * refresh / (dT * NUM);

    float xCurr = xRefresh + min.x;
    while (xCurr < max.x) {
      // use ceil to get integer coords or else lines look odd
      float xP = std::ceil(xCurr);
      draw->AddLine(ImVec2(xP, min.y), ImVec2(xP, max.y), 0x50FFFFFF);
      xCurr += xRefresh;
    }

    if (pValues_) {
      // calc markers
      float currTotal = 0.0f;
      int mark        = 0;
      for (int i = 0; i < NUM && mark < NUM_MARKERS; ++i) {
        currTotal += pValues_[i];
        if (total_ * markers[mark] < currTotal) {
          float xP = std::ceil(
            static_cast<float>(i + 1) / static_cast<float>(NUM)
              * (max.x - min.x)
            + min.x); // use ceil to get integer coords or else lines look odd
          draw->AddLine(ImVec2(xP, min.y), ImVec2(xP, max.y), 0xFFFF0000);
          ++mark;
        }
      }
    }
  }

  void CalcHistogramSize(int numShown_)
  {
    ImVec2 wRegion   = ImGui::GetContentRegionMax();
    float heightGone = 7.0f * ImGui::GetItemsLineHeightWithSpacing();
    wRegion.y -= heightGone;
    wRegion.y /= static_cast<float>(numShown_);
    const ImGuiStyle& style = ImGui::GetStyle();
    ImVec2 pad              = style.FramePadding;
    wRegion.x -= 2.0f * pad.x;
    size = wRegion;
  }

  void Draw(const char* name_, bool* pOpen_ = nullptr)
  {
    if (ImGui::Begin(name_, pOpen_)) {
      int numShown = 0;
      if (ImGui::CollapsingHeader("Time Histogram")) {
        ++numShown;
        ImGui::PlotHistogram("", times, NUM, 0, nullptr, FLT_MAX, FLT_MAX,
                             size);
        PlotRefreshLines(timesTotal, times);
      }
      if (ImGui::CollapsingHeader("Count Histogram")) {
        ++numShown;
        ImGui::PlotHistogram("", counts, NUM, 0, nullptr, FLT_MAX, FLT_MAX,
                             size);
        PlotRefreshLines(countsTotal, counts);
      }
      if (ImGui::CollapsingHeader("Hitch Time Histogram")) {
        ++numShown;
        ImGui::PlotHistogram("", hitchTimes, NUM, 0, nullptr, FLT_MAX, FLT_MAX,
                             size);
        PlotRefreshLines();
      }
      if (ImGui::CollapsingHeader("Hitch Count Histogram")) {
        ++numShown;
        ImGui::PlotHistogram("", hitchCounts, NUM, 0, nullptr, FLT_MAX, FLT_MAX,
                             size);
        PlotRefreshLines();
      }
      if (ImGui::Button("Clear")) {
        Clear();
      }
      CalcHistogramSize(numShown);
    }
    ImGui::End();
  }
}; // end of struct FrameTimeHistogram

} // namespace ImGui
