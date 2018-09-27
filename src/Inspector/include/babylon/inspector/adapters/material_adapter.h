#ifndef BABYLON_INSPECTOR_ADAPTERS_MATERIAL_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_MATERIAL_ADAPTER_H

#include <babylon/inspector/adapters/adapter.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

namespace BABYLON {

class Material;
class StandardMaterial;
using MaterialPtr         = std::shared_ptr<Material>;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

class MaterialAdapter : public Adapter {

public:
  MaterialAdapter(const MaterialPtr& material);
  ~MaterialAdapter() override;

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
   * @brief No tools for a material adapter.
   */
  std::vector<AbstractTreeToolUPtr>& getTools() override;

private:
  void _buildPropertiesView();

private:
  MaterialPtr _material;
  StandardMaterialPtr _standardMaterial;
  std::unique_ptr<PropertiesView> _properties;
  std::vector<std::unique_ptr<AbstractTreeTool>> _tools;

}; // end of class MaterialAdapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_MATERIAL_ADAPTER_H
