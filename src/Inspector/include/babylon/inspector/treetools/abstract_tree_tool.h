#ifndef BABYLON_INSPECTOR_TREE_TOOLS_ABSTRACT_TREE_TOOL_H
#define BABYLON_INSPECTOR_TREE_TOOLS_ABSTRACT_TREE_TOOL_H

namespace BABYLON {

class AbstractTreeTool {

public:
  AbstractTreeTool();
  virtual ~AbstractTreeTool();

  AbstractTreeTool(const AbstractTreeTool& other) = delete;
  AbstractTreeTool(AbstractTreeTool&& other);

  virtual void render() = 0;

protected:
  /**
   * @brief Action launched when clicked on this element.
   * Should be overrided
   */
  virtual void action();

protected:
  /** Is the tool enabled ? */
  bool _on;

}; // end of struct IToolVisible

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TOOLS_ABSTRACT_TREE_TOOL_H
