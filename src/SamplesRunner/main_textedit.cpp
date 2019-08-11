#include <fstream>
#include "chdir_to_exe.h"
#include "TextEditor.h"
#include <imgui_utils/app_runner/imgui_runner.h>

static const char* fileToEdit = "F:/dvp/OpenSource/BabylonCpp/external/ImGuiColorTextEdit/TextEditor.cpp";

TextEditor CreateTextEditor()
{
  ///////////////////////////////////////////////////////////////////////
// TEXT EDITOR SAMPLE
  TextEditor editor;
  auto lang = TextEditor::LanguageDefinition::CPlusPlus();

  // set your own known preprocessor symbols...
  static const char* ppnames[] = { "NULL", "PM_REMOVE" };
  // ... and their corresponding values
  static const char* ppvalues[] = {
    "#define NULL ((void*)0)",
    "#define PM_REMOVE (0x0001)"
  };

  for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
  {
    TextEditor::Identifier id;
    id.mDeclaration = ppvalues[i];
    lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
  }

  // set your own identifiers
  static const char* identifiers[] = {
    "HWND", "HRESULT", "LPRESULT"};
  static const char* idecls[] = { "typedef HWND_* HWND", "typedef long HRESULT"};
  for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
  {
    TextEditor::Identifier id;
    id.mDeclaration = std::string(idecls[i]);
    lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
  }
  editor.SetLanguageDefinition(lang);
  //editor.SetPalette(TextEditor::GetLightPalette());

  // error markers
  TextEditor::ErrorMarkers markers;
  markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
  markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
  editor.SetErrorMarkers(markers);

  // "breakpoint" markers
  //TextEditor::Breakpoints bpts;
  //bpts.insert(24);
  //bpts.insert(47);
  //editor.SetBreakpoints(bpts);

  {
    std::ifstream t(fileToEdit);
    if (t.good())
    {
      std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
      editor.SetText(str);
    }
  }
  return editor;
}


bool render(TextEditor & editor)
{
  ImGui::ShowTestWindow();

  auto cpos = editor.GetCursorPosition();
  ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
  ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Save"))
      {
        auto textToSave = editor.GetText();
        /// save text....
      }
      if (ImGui::MenuItem("Quit", "Alt-F4"))
        return true;
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit"))
    {
      bool ro = editor.IsReadOnly();
      if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
        editor.SetReadOnly(ro);
      ImGui::Separator();

      if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
        editor.Undo();
      if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
        editor.Redo();

      ImGui::Separator();

      if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
        editor.Copy();
      if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
        editor.Cut();
      if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
        editor.Delete();
      if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
        editor.Paste();

      ImGui::Separator();

      if (ImGui::MenuItem("Select all", nullptr, nullptr))
        editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
      if (ImGui::MenuItem("Dark palette"))
        editor.SetPalette(TextEditor::GetDarkPalette());
      if (ImGui::MenuItem("Light palette"))
        editor.SetPalette(TextEditor::GetLightPalette());
      if (ImGui::MenuItem("Retro blue palette"))
        editor.SetPalette(TextEditor::GetRetroBluePalette());
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
    editor.IsOverwrite() ? "Ovr" : "Ins",
    editor.CanUndo() ? "*" : " ",
    editor.GetLanguageDefinition().mName.c_str(), fileToEdit);

  editor.Render("TextEditor");
  ImGui::End();

  return false;
}


int main2(int, char** argv)
{
  auto textEditor = CreateTextEditor();
  ChdirToExe(argv[0]);
  auto initFunction = [&]() {
  };

  auto showGui = [&]() {
    render(textEditor);
  };

  ImGuiUtils::ImGuiRunner::AppWindowParams params;
  //params.FullScreen = true;
  params.Width = 1280;
  params.Height = 720;
  params.LoadFontAwesome = false;
  params.Title = "Hello app";
  ImGuiUtils::ImGuiRunner::RunGui(showGui, params, initFunction);
  return 0;
}
