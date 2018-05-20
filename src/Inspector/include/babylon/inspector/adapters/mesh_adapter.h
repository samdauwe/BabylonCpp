#ifndef BABYLON_INSPECTOR_ADAPTERS_MESH_ADAPTER_H
#define BABYLON_INSPECTOR_ADAPTERS_MESH_ADAPTER_H

#include <babylon/babylon_stl.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/observer.h>

#include <babylon/inspector/adapters/adapter.h>
#include <babylon/inspector/treetools/itool_bounding_box.h>
#include <babylon/inspector/treetools/itool_debug.h>
#include <babylon/inspector/treetools/itool_info.h>
#include <babylon/inspector/treetools/itool_visible.h>

namespace BABYLON {

class AbstractMesh;
class Node;

namespace Debug {
class AxesViewer;
} // end of namespace Debug

class MeshAdapter : public Adapter,
                    public IToolVisible,
                    public IToolDebug,
                    public IToolBoundingBox,
                    public IToolInfo {

public:
  MeshAdapter(Node* obj);
  ~MeshAdapter() override;

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
   * @brief Returns the list of tools available for this adapter.
   */
  vector_t<AbstractTreeToolUPtr>& getTools() override;

  void setVisible(bool b) override;

  bool isVisible() const override;

  bool isBoxVisible() const override;

  void setBoxVisible(bool b) override;

  void debug(bool enable) override;

  /**
   * @brief Returns some information about this mesh.
   */
  string_t getInfo() const override;

private:
  /**
   * @brief Draw X, Y and Z axis for the actual object if this adapter.
   * Should be called only one time as it will fill _axis
   */
  void _drawAxis();

  /**
   * @brief Build the properties view.
   */
  void _buildPropertiesView();

private:
  Node* _obj;
  AbstractMesh* _abstractMesh;
  Mesh* _mesh;
  GroundMesh* _groundMesh;
  unique_ptr_t<PropertiesView> _properties;
  vector_t<unique_ptr_t<AbstractTreeTool>> _tools;
  /** Keep track of the axis of the actual object */
  unique_ptr_t<Debug::AxesViewer> _axesViewer;
  Observer<Scene>::Ptr onBeforeRenderObserver;
  // Axis
  Vector3 _x, _y, _z;

}; // end of class MeshAdapter

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_ADAPTERS_MESH_ADAPTER_H
