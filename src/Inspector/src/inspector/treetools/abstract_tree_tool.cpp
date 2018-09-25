#include <babylon/inspector/treetools/abstract_tree_tool.h>

#include <babylon/babylon_stl.h>

namespace BABYLON {

AbstractTreeTool::AbstractTreeTool() : _on{false}
{
}

AbstractTreeTool::~AbstractTreeTool()
{
}

AbstractTreeTool::AbstractTreeTool(AbstractTreeTool&& other)
    : _on{std::move(other._on)}
{
}

void AbstractTreeTool::action()
{
  _on = !_on;
}

} // end of namespace BABYLON
