#ifndef BABYLON_INSPECTOR_ADAPTERS_LIGHT_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_LIGHT_ADAPTER_H

#include <babylon/babylon_stl.h>

#include <babylon/inspector/adapters/adapter.h>
#include <babylon/inspector/treetools/itool_visible.h>

namespace BABYLON {

class Light;

class LightAdapter : public Adapter, public IToolVisible {

public:
  LightAdapter(Light* light);
  ~LightAdapter() override;

  /**
   * @brief Returns the name displayed in the tree.
   */
  string_t id() override;

  /**
   * @brief Returns the type of this object - displayed in the tree.
   */
  string_t type() override;

  vector_t<AbstractTreeToolUPtr>& getTools() override;

  void setVisible(bool b) override;

  bool isVisible() const override;

private:
  Light* _light;
  vector_t<unique_ptr_t<AbstractTreeTool>> _tools;

}; // end of class LightAdapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_LIGHT_ADAPTER_H
