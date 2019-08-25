#pragma once

#include <cmath>
#include <imgui.h>
#include <imgui_internal.h>

#ifdef _MSC_VER
#pragma warning(disable: 4996) // warning C4996: 'strncpy and co': This function or variable may be unsafe.
#endif

// from https://github.com/juliettef/IconFontCppHeaders
#include <imgui_utils/icons_font_awesome_5.h>

namespace ImGui {

static constexpr float IconSize       = 9.f;
static constexpr float IconSizeDouble = 20.f;
static constexpr float IconSizeHalf   = 4.5f;

static constexpr float IsNotActiveOpacity = 0.5f;

static ImVec4 cornflowerblue = ImColor(0.39f, 0.58f, 0.93f, 1.0f); // #6495ED
static ImVec4 dodgerblue     = ImColor(0.12f, 0.56f, 1.00f, 1.0f); // #1E90FF
static ImVec4 gray           = ImColor(0.50f, 0.50f, 0.50f, 1.0f); // #808080
static ImVec4 green          = ImColor(0.00f, 0.50f, 0.00f, 1.0f); // #008000
static ImVec4 lightgray      = ImColor(0.83f, 0.83f, 0.83f, 1.0f); // #D3D3D3
static ImVec4 orange         = ImColor(1.00f, 0.65f, 0.00f, 1.0f); // #FFA500
static ImVec4 orangered      = ImColor(1.00f, 0.27f, 0.00f, 1.0f); // #FF4500
static ImVec4 mediumpurple   = ImColor(0.58f, 0.44f, 0.86f, 1.0f); // #9370DB
static ImVec4 red            = ImColor(1.00f, 0.00f, 0.00f, 1.0f); // #FF0000
static ImVec4 yellow         = ImColor(1.00f, 1.00f, 0.00f, 1.0f); // #FFFF00

/**
 * @brief Same as ImGui original InputText, but works with an in/out std::string
 * @return color "color-bot"
 */
inline bool InputText_String(
  const std::string &label, 
  std::string & inOutStr,
  ImGuiInputTextFlags flags = 0,
  ImGuiInputTextCallback callback = NULL, 
  void* user_data = NULL
  )
{
  size_t buf_size = inOutStr.size() + 1000;
  char *buf = (char *)malloc(buf_size);
  strcpy(buf, inOutStr.c_str());
  bool changed = ImGui::InputText(label.c_str(), buf, buf_size, flags, callback, user_data);
  inOutStr = std::string(buf);
  free(buf);
  return changed;
}

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
#if 0
  style.Colors[ImGuiCol_Column]            = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_ColumnHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  style.Colors[ImGuiCol_ColumnActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
#endif
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

const char ColorMarkerStart = '{';
const char ColorMarkerEnd   = '}';

inline bool ProcessInlineHexColor(const char* start, const char* end,
                                  ImVec4& color)
{
  const int hexCount = (int)(end - start);
  if (hexCount == 6 || hexCount == 8) {
    char hex[9];
    strncpy(hex, start, hexCount);
    hex[hexCount] = 0;

    unsigned int hexColor = 0;
    if (sscanf(hex, "%x", &hexColor) > 0) {
      color.x = static_cast<float>((hexColor & 0x00FF0000) >> 16) / 255.0f;
      color.y = static_cast<float>((hexColor & 0x0000FF00) >> 8) / 255.0f;
      color.z = static_cast<float>((hexColor & 0x000000FF)) / 255.0f;
      color.w = 1.0f;

      if (hexCount == 8) {
        color.w = static_cast<float>((hexColor & 0xFF000000) >> 24) / 255.0f;
      }

      return true;
    }
  }

  return false;
}

/**
 * @brief Render colored text
 * @see https://github.com/ocornut/imgui/issues/902
 */
inline void TextWithColors(const char* fmt, ...)
{
  char tempStr[4096];

  va_list argPtr;
  va_start(argPtr, fmt);
  // _vsnprintf( tempStr, sizeof( tempStr ), fmt, argPtr );
  vsprintf(tempStr, fmt, argPtr);
  va_end(argPtr);
  tempStr[sizeof(tempStr) - 1] = '\0';

  bool pushedColorStyle = false;
  const char* textStart = tempStr;
  const char* textCur   = tempStr;
  while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0') {
    if (*textCur == ColorMarkerStart) {
      // Print accumulated text
      if (textCur != textStart) {
        ImGui::TextUnformatted(textStart, textCur);
        ImGui::SameLine(0.0f, 0.0f);
      }

      // Process color code
      const char* colorStart = textCur + 1;
      do {
        ++textCur;
      } while (*textCur != '\0' && *textCur != ColorMarkerEnd);

      // Change color
      if (pushedColorStyle) {
        ImGui::PopStyleColor();
        pushedColorStyle = false;
      }

      ImVec4 textColor;
      if (ProcessInlineHexColor(colorStart, textCur, textColor)) {
        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
        pushedColorStyle = true;
      }

      textStart = textCur + 1;
    }
    else if (*textCur == '\n') {
      // Print accumulated text an go to next line
      ImGui::TextUnformatted(textStart, textCur);
      textStart = textCur + 1;
    }

    ++textCur;
  }

  if (textCur != textStart) {
    ImGui::TextUnformatted(textStart, textCur);
  }
  else {
    ImGui::NewLine();
  }

  if (pushedColorStyle) {
    ImGui::PopStyleColor();
  }
}

/**
 * @brief Splitter implementation using SplitterBehavior in imgui_internal.h as
 * a helper.
 * @see https://github.com/ocornut/imgui/issues/319
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
  return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y,
                          size1, size2, min_size1, min_size2, 0.0f);
}

/**
 * @brief SliderFloat implementation with steps.
 * @see https://github.com/ocornut/imgui/issues/1183
 */
inline bool SliderFloatWithSteps(const char* label, float* v, float v_min,
                                 float v_max, float v_step,
                                 const char* display_format = nullptr)
{
  if (!display_format) {
    display_format = "%.3f";
  }

  char text_buf[64] = {};
  ImFormatString(text_buf, IM_ARRAYSIZE(text_buf), display_format, *v);

  // Map from [v_min,v_max] to [0,N]
  const int countValues    = int((v_max - v_min) / v_step);
  int v_i                  = int((*v - v_min) / v_step);
  const bool value_changed = SliderInt(label, &v_i, 0, countValues, text_buf);

  // Remap from [0,N] to [v_min,v_max]
  *v = v_min + float(v_i) * v_step;
  return value_changed;
}

/**
 * @brief Toggle Button
 * @see https://github.com/ocornut/imgui/issues/1537
 */
inline void SwitchButton(const char* str_id, bool* v)
{
  ImGuiStyle& style = ImGui::GetStyle();

  ImVec2 p              = ImGui::GetCursorScreenPos();
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  float height = ImGui::GetFrameHeight();
  float width  = height * 1.55f;
  float radius = height * 0.50f;

  ImGui::InvisibleButton(str_id, ImVec2(width, height));
  if (ImGui::IsItemClicked()) {
    *v = !*v;
  }

  float t = (*v) ? 1.0f : 0.0f;

  ImGuiContext& g  = *GImGui;
  float ANIM_SPEED = 0.08f;
  if (g.LastActiveId
      == g.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
  {
    float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
    t            = (*v) ? (t_anim) : (1.0f - t_anim);
  }

  ImU32 col_bg;
  auto bgColor = (*v) ? style.Colors[ImGuiCol_TitleBgActive] :
                        ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  if (ImGui::IsItemHovered()) {
    col_bg = ImGui::GetColorU32(ImLerp(bgColor, bgColor, t));
  }
  else {
    col_bg = ImGui::GetColorU32(ImLerp(bgColor, bgColor, t));
  }

  draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg,
                           height * 0.5f);
  draw_list->AddCircleFilled(
    ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius),
    radius - 1.5f, IM_COL32(255, 255, 255, 255));
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
  return CheckBoxFont(name_, pB_, ICON_FA_CHECK_SQUARE, ICON_FA_SQUARE);
}

inline bool MenuItemCheckbox(const char* name_, bool* pB_)
{
  bool retval = ImGui::MenuItem(name_);
  ImGui::SameLine();
  if (*pB_) {
    ImGui::Text(ICON_FA_CHECK_SQUARE);
  }
  else {
    ImGui::Text(ICON_FA_SQUARE);
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

namespace ImGuiUtils
{
  void ImageFromFile(const std::string &filename, ImVec2 size = ImVec2(0.f, 0.f));

  inline bool Button_WithEnable(const char* label, bool enabled, const ImVec2& size = ImVec2(0, 0))
  {
    if (enabled)
      return ImGui::Button(label, size);
    else
    {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.4f);
      bool r = ImGui::Button(label, size);
      (void)r; // for disabled buttons, the result is always dismissed
      ImGui::PopStyleVar();
      return false;
    }
  }
}