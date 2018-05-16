#ifndef BABYLON_INSPECTOR_ADAPTERS_MATERIAL_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_MATERIAL_ADAPTER_H

#include <babylon/babylon_stl.h>

#include <babylon/inspector/adapters/adapter.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

namespace BABYLON {

class Material;
class StandardMaterial;

class MaterialAdapter : public Adapter {

public:
  MaterialAdapter(Material* material);
  ~MaterialAdapter() override;

  /**
   * @brief Returns the name displayed in the tree.
   */
  string_t id() override;

  /**
   * @brief Returns the type of this object - displayed in the tree.
   */
  string_t type() override;

  /**
   * @brief Returns the list of properties to be displayed for this adapter.
   */
  unique_ptr_t<PropertiesView>& getProperties() override;

  /**
   * @brief No tools for a material adapter.
   */
  vector_t<AbstractTreeToolUPtr>& getTools() override;

private:
  void _buildPropertiesView();

private:
  Material* _material;
  StandardMaterial* _standardMaterial;
  unique_ptr_t<PropertiesView> _properties;
  vector_t<unique_ptr_t<AbstractTreeTool>> _tools;

}; // end of class MaterialAdapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_MATERIAL_ADAPTER_H
