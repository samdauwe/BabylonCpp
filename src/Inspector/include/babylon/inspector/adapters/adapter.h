#ifndef BABYLON_INSPECTOR_ADAPTERS_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_ADAPTER_H

#include <babylon/babylon_stl.h>

namespace BABYLON {

class AbstractTreeTool;
class PropertiesView;

class Adapter {

public:
  using AbstractTreeToolUPtr = unique_ptr_t<AbstractTreeTool>;

public:
  Adapter();
  virtual ~Adapter();

  /**
   * @brief Returns the name displayed in the tree.
   */
  virtual string_t id() = 0;

  /**
   * @brief Returns the type of this object - displayed in the tree.
   */
  virtual string_t type() = 0;

  /**
   * @brief Returns the list of properties to be displayed for this adapter.
   */
  virtual unique_ptr_t<PropertiesView>& getProperties() = 0;

  /**
   * @brief Returns the list of tools available for this adapter.
   */
  virtual vector_t<AbstractTreeToolUPtr>& getTools() = 0;

protected:
  /**
   * @brief Returns the adapter unique name.
   */
  string_t get_name() const;

public:
  /**
   * Adapter unique name.
   */
  ReadOnlyProperty<Adapter, string_t> name;

private:
  // a unique name for this adapter, to retrieve its own key in the local
  // storage
  static string_t _name;

}; // end of class Adapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_ADAPTER_H
