#include <string>

namespace ImGuiUtils {

  class MultipleCodeEditorImpl;

  class MultipleCodeEditor
  {
  public:
    MultipleCodeEditor(const std::string &filePath);
    MultipleCodeEditor(const std::vector<std::string> &filePaths);
    ~MultipleCodeEditor();

    bool render();

  private:
    std::unique_ptr<MultipleCodeEditorImpl> _pImpl;
  };

  // Call this from main() to get a working demo
  void demoCodeEditor();

} // namespace ImGuiUtils
