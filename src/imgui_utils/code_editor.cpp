#include <functional>
#include <string>
#include <iostream>
#include <babylon/core/filesystem.h>
#include <babylon/core/string.h>
#include <babylon/core/system.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <imgui_utils/imgui_utils.h>
#include <ImGuiColorTextEdit/TextEditor.h>
#include <imgui_utils/code_editor.h>
#include <imgui_utils/app_runner/imgui_runner.h>

namespace ImGuiUtils {

std::function<void(void)> guiAllowEdition;

namespace
{
  std::string readFileContents_standard_eof(const std::string& filePath)
  {
    std::string s = BABYLON::Filesystem::readFileContents(filePath.c_str());
    s = BABYLON::String::replace(s, "\r\n", "\n");
    return s;
  }

}

class OneCodeEditor
{
public:
  OneCodeEditor(const std::string &filePath) : _filePath(filePath)
  {
    auto lang = TextEditor::LanguageDefinition::CPlusPlus();
    _textEditor.SetLanguageDefinition(lang);
    readFile();
  }

  void OneCodeEditor::setLightPalette()
  {
    _textEditor.SetPalette(TextEditor::GetLightPalette());
  }

  void readFile()
  {
    _fileContent_Startup = readFileContents_standard_eof(_filePath);
    _fileContent_Saved = _fileContent_Startup;
    _textEditor.SetText(_fileContent_Startup);
  }

  void render()
  {
    //ImGui::ShowDemoWindow();
    checkExternalModifications();
    renderExternalEdition();
    renderCommandLine();
    ImGui::Separator();
    renderStatusLine();
    ImGui::Separator();
    if (_hasModificationConflict)
    {
      ImGui::OpenPopup("Edition Conflict!");
      if (ImGui::BeginPopupModal("Edition Conflict!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
      {
        ImGui::Text("This file was concurrently modified externally and edited here!");
        if (ImGui::Button("Accept external changes"))
        {
          readFile();
          _hasModificationConflict = false;
        }
        ImGui::EndPopup();
      }

    }
    _textEditor.Render("TextEditor");
  }

  std::string filePath() const { return _filePath; }
  std::string baseName() const {
    return BABYLON::Filesystem::baseName(_filePath);
  }
  TextEditor & getTextEditor() { return _textEditor;  }

  void save()
  {
    std::string text = _textEditor.GetText();
    // The code editor may add an unwanted additional 
    // empty line at the end of the text
    if (!text.empty())
    {
      if (text[text.size() - 1] == '\n')
        text = text.substr(0, text.size() - 1);
    }
    BABYLON::Filesystem::writeFileContents(_filePath.c_str(), text);
    _fileContent_Saved = text;
  }

private:
  bool canSave()
  {
    if (_textEditor.IsReadOnly())
      return false;
    return wasTextEdited();
  }
  bool canRestore()
  {
    if (_textEditor.IsReadOnly())
      return false;
    return _textEditor.CanUndo();
  }
  void restore()
  {
    if (_textEditor.IsReadOnly())
      return;
    _textEditor.SetText(_fileContent_Startup);
  }

  void renderCommandLine()
  {
    if (!_textEditor.IsReadOnly())
    {
      if (ImGuiUtils::Button_WithEnable(ICON_FA_SAVE " Save", canSave()))
      {
        save();
      }
      ImGui::SameLine();
      if (ImGuiUtils::Button_WithEnable(ICON_FA_UNDO ICON_FA_UNDO " Restore", canRestore()))
        restore();
      ImGui::SameLine(0.f, 50.f);

      if (ImGuiUtils::Button_WithEnable(ICON_FA_UNDO, _textEditor.CanUndo()))
        _textEditor.Undo();
      ImGui::SameLine();

      if (ImGuiUtils::Button_WithEnable(ICON_FA_REDO, _textEditor.CanRedo()))
        _textEditor.Redo();
      ImGui::SameLine();

      if (ImGuiUtils::Button_WithEnable(ICON_FA_COPY, _textEditor.HasSelection()))
        _textEditor.Copy();
      ImGui::SameLine();
      if (ImGuiUtils::Button_WithEnable(ICON_FA_CUT, _textEditor.HasSelection()))
        _textEditor.Cut();
      ImGui::SameLine();
      if (ImGuiUtils::Button_WithEnable(ICON_FA_PASTE, ImGui::GetClipboardText() != nullptr))
        _textEditor.Paste();
    }

  }

  void renderStatusLine()
  {
    auto cpos = _textEditor.GetCursorPosition();
    bool isTextModified = (_fileContent_Saved != _textEditor.GetText());
    ImGui::TextWrapped("%s", _filePath.c_str());
    ImGui::Text("%6d/%-6d %6d lines | %s | %s | %s ", cpos.mLine + 1, cpos.mColumn + 1, _textEditor.GetTotalLines(),
      isTextModified ? "*" : " ",
      _textEditor.GetLanguageDefinition().mName.c_str(),
      _textEditor.IsOverwrite() ? "Ovr" : "Ins"
      );
  }

  void renderExternalEdition()
  {
    if (guiAllowEdition)
      guiAllowEdition();
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_EDIT "external editor"))
      BABYLON::System::openFile(_filePath);
  }

  bool wasTextEdited()
  {
    bool wasEdited = true;
    std::string currentText = _textEditor.GetText();
    if (currentText == _fileContent_Saved)
      wasEdited = false;

    // The editor can add a final "EOL" that is not present in the file...
    currentText.pop_back();
    if (currentText == _fileContent_Saved)
      wasEdited = false;

    return wasEdited;
  }

  void checkExternalModifications()
  {
    static int counter = 0;
    counter++;
    // only check every second
    if (counter % 50 == 0)
    {
      std::string newContent = readFileContents_standard_eof(_filePath.c_str());
      bool wasExternallyModified = (newContent != _fileContent_Saved);
      bool wasInternallyModified = wasTextEdited();
      if (wasExternallyModified && (!wasInternallyModified))
      {
        _textEditor.SetText(newContent);
        _fileContent_Saved = newContent;
      }
      if (wasExternallyModified && wasInternallyModified)
      {
        _hasModificationConflict = true;
        std::cerr << "File was modified externally and internally!";
      }
    }
  }

  std::string _filePath;
  TextEditor _textEditor;
  std::string _fileContent_Startup;
  std::string _fileContent_Saved;
  bool _hasModificationConflict = false;
};



class MultipleCodeEditorImpl
{
public:
  MultipleCodeEditorImpl(bool showCheckboxReadOnly)
    : _showCheckboxReadOnly(showCheckboxReadOnly)
  {
    if (!_showCheckboxReadOnly)
      _canEdit = true;
  }

  void setFiles(const std::vector<std::string> &filePaths)
  {
    _editors.clear();
    for (auto filePath : filePaths)
      _editors.push_back(OneCodeEditor(filePath));
    if (!_editors.empty())
      _currentEditor = &_editors.back();

    guiAllowEdition = [this]() {
      this->renderAllowEdition();
    };

    updateReadOnly();
  }

  void updateReadOnly()
  {
    for (auto & editor : _editors)
    {
      editor.getTextEditor().SetReadOnly(!_canEdit);
    }
  }

  void renderAllowEdition()
  {
    if (_showCheckboxReadOnly)
    {
      if (ImGui::Checkbox("Allow edition", &_canEdit))
        updateReadOnly();
    }
  }

  void render()
  {
    if (_editors.empty())
      return;

    renderTabs();
    //ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 200.f);
    // renderPalette();
    if (_currentEditor)
      currentEditor().render();
  }

  void saveAll()
  {
    for (auto & editor : _editors)
    {
      editor.save();
    }
  }

  void setLightPalette()
  {
    for (auto& editor : _editors)
      editor.setLightPalette();
  }

private:
  void renderTabs()
  {
    if (_editors.size() < 2)
      return;
    for (auto & editor : _editors)
    {
      bool isCurrent = (_currentEditor == &editor);
      if (isCurrent)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.6f, 1.f));
      if (ImGui::Button(editor.baseName().c_str()))
      {
        _currentEditor = &editor;
      }
      if (isCurrent)
        ImGui::PopStyleColor();
      ImGui::SameLine();
    }
    ImGui::NewLine();
  }

  void renderPalette()
  {
    //ImGui::Text("Palette"); ImGui::SameLine();
    if (ImGui::RadioButton("Dark", &_palette, 0))
      for (auto & editor : _editors)
        editor.getTextEditor().SetPalette(TextEditor::GetDarkPalette());
    ImGui::SameLine();
    if (ImGui::RadioButton("Light", &_palette, 1))
      for (auto & editor : _editors)
        editor.getTextEditor().SetPalette(TextEditor::GetLightPalette());
    ImGui::SameLine();
    if (ImGui::RadioButton("Blue", &_palette, 2))
      for (auto & editor : _editors)
        editor.getTextEditor().SetPalette(TextEditor::GetRetroBluePalette());
  }


  OneCodeEditor & currentEditor()
  {
    return *_currentEditor;
  }

  std::vector<OneCodeEditor> _editors;
  OneCodeEditor * _currentEditor = nullptr;
  int _palette = 0;
  bool _canEdit = false;
  bool _showCheckboxReadOnly;
};




CodeEditor::CodeEditor(bool showCheckboxReadOnly)
  : _pImpl(std::make_unique<MultipleCodeEditorImpl>(showCheckboxReadOnly))
{
}

void CodeEditor::setFiles(const std::vector<std::string> &filePaths)
{
  _pImpl->setFiles(filePaths);
  _isEmpty = false;
}

CodeEditor::~CodeEditor() = default;

void CodeEditor::render()
{
  return _pImpl->render();
}


void CodeEditor::saveAll()
{
  _pImpl->saveAll();
}

void CodeEditor::setLightPalette()
{
  _pImpl->setLightPalette();
}

void demoCodeEditor()
{
  std::vector<std::string> path =
  {
    "../../../src/imgui_babylon/src/imgui_babylon/sample_list_page.cpp",
    "../../../src/imgui_babylon/include/babylon/imgui_babylon/sample_list_page.h"
  };
  ImGuiUtils::CodeEditor codeEditor(true);
  codeEditor.setFiles(path);
  auto demoGui = [&]() {
    codeEditor.render();
  };

  ImGuiUtils::ImGuiRunner::AppWindowParams params;
  //params.FullScreen = true;
  params.Title = "Hello World";
  params.ShowMenuBar = true;
  ImGuiUtils::ImGuiRunner::RunGui(demoGui, params);

}

} // namespace ImGuiUtils
