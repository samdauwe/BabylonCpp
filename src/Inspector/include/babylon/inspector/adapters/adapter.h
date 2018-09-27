#ifndef BABYLON_INSPECTOR_ADAPTERS_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_ADAPTER_H

#include <memory>

#include <babylon/babylon_common.h>

namespace BABYLON {

class AbstractTreeTool;
class PropertiesView;

class Adapter {

public:
  using AbstractTreeToolUPtr = std::unique_ptr<AbstractTreeTool>;

public:
  Adapter();
  virtual ~Adapter();

  /**
   * @brief Returns the name displayed in the tree.
   */
  virtual std::string id() = 0;

  /**
   * @brief Returns the type of this object - displayed in the tree.
   */
  virtual std::string type() = 0;

  /**
   * @brief Returns the list of properties to be displayed for this adapter.
   */
  virtual std::unique_ptr<PropertiesView>& getProperties() = 0;

  /**
   * @brief Returns the list of tools available for this adapter.
   */
  virtual std::vector<AbstractTreeToolUPtr>& getTools() = 0;

protected:
  /**
   * @brief Returns the adapter unique name.
   */
  std::string get_name() const;

public:
  /**
   * Adapter unique name.
   */
  ReadOnlyProperty<Adapter, std::string> name;

private:
  // a unique name for this adapter, to retrieve its own key in the local
  // storage
  static std::string _name;

}; // end of class Adapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_ADAPTER_H
