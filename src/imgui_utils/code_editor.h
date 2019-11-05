#ifndef IMGUI_UTILS_CODEEDITOR_H
#define IMGUI_UTILS_CODEEDITOR_H

#include <string>

namespace ImGuiUtils {

  class MultipleCodeEditorImpl;

  class CodeEditor
  {
  public:
    CodeEditor(bool showCheckboxReadOnly = false);
    ~CodeEditor();

    void setFiles(const std::vector<std::string> &filePaths);
    void render();
    void saveAll();
    bool isEmpty() { return _isEmpty; }
    void setLightPalette();

  private:
    std::unique_ptr<MultipleCodeEditorImpl> _pImpl;
    bool _isEmpty = true;
  };

  // Call this from main() to get a working demo
  void demoCodeEditor();

} // namespace ImGuiUtils

#endif // #ifndef IMGUI_UTILS_CODEEDITOR_H
