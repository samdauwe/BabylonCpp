#ifndef BABYLON_INSPECTOR_ADAPTERS_LIGHT_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_LIGHT_ADAPTER_H

#include <babylon/inspector/adapters/adapter.h>
#include <babylon/inspector/treetools/itool_visible.h>

namespace BABYLON {

class HemisphericLight;
class Light;
using HemisphericLightPtr = std::shared_ptr<HemisphericLight>;
using LightPtr            = std::shared_ptr<Light>;

class LightAdapter : public Adapter, public IToolVisible {

public:
  LightAdapter(const LightPtr& light);
  ~LightAdapter() override;

  /**
   * @brief Returns the name displayed in the tree.
   */
  std::string id() override;

  /**
   * @brief Returns the type of this object - displayed in the tree.
   */
  std::string type() override;

  /**
   * @brief Returns the list of properties to be displayed for this adapter.
   */
  std::unique_ptr<PropertiesView>& getProperties() override;

  /**
   * @brief Returns the list of tools available for this adapter.
   */
  std::vector<AbstractTreeToolUPtr>& getTools() override;

  void setVisible(bool b) override;

  bool isVisible() const override;

private:
  void _buildPropertiesView();

private:
  LightPtr _light;
  HemisphericLightPtr _hemispericLight;
  std::unique_ptr<PropertiesView> _properties;
  std::vector<std::unique_ptr<AbstractTreeTool>> _tools;

}; // end of class LightAdapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_LIGHT_ADAPTER_H
