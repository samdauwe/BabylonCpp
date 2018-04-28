#include <babylon/inspector/adapters/mesh_adapter.h>

#include <babylon/debug/axes_viewer.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>

#include <babylon/inspector/treetools/bounding_box.h>
#include <babylon/inspector/treetools/checkbox.h>
#include <babylon/inspector/treetools/debug_area.h>
#include <babylon/inspector/treetools/info.h>

namespace BABYLON {

MeshAdapter::MeshAdapter(Node* obj)
    : _obj{obj}
    , _axesViewer{nullptr}
    , onBeforeRenderObserver{nullptr}
    , _x{1.f, 0.f, 0.f}
    , _y{0.f, 1.f, 0.f}
    , _z{0.f, 0.f, 1.f}
{
  _tools.emplace_back(::std::make_unique<Checkbox>(this));
  _tools.emplace_back(::std::make_unique<DebugArea>(this));
  _mesh = dynamic_cast<AbstractMesh*>(obj);
  if (_mesh) {
    if (_mesh->getTotalVertices() > 0) {
      _tools.emplace_back(::std::make_unique<BoundingBoxInsp>(this));
    }
  }
  _tools.emplace_back(::std::make_unique<Info>(this));
}

MeshAdapter::~MeshAdapter()
{
}

string_t MeshAdapter::id()
{
  return _obj->name;
}

string_t MeshAdapter::type()
{
  return _obj->getClassName();
}

vector_t<unique_ptr_t<AbstractTreeTool>>& MeshAdapter::getTools()
{
  return _tools;
}

void MeshAdapter::setVisible(bool b)
{
  _obj->setEnabled(b);
  if (_mesh) {
    _mesh->isVisible = b;
  }
}

bool MeshAdapter::isVisible() const
{
  return _obj->isEnabled() && (_mesh && _mesh->isVisible);
}

bool MeshAdapter::isBoxVisible() const
{
  return _mesh ? _mesh->showBoundingBox : false;
}

void MeshAdapter::setBoxVisible(bool b)
{
  if (_mesh) {
    _mesh->showBoundingBox = b;
  }
}

void MeshAdapter::debug(bool enable)
{
  // Draw axis the first time
  if (!_axesViewer) {
    _drawAxis();
  }
  // Display or hide axis
  if (!enable && _axesViewer) {
    if (_mesh) {
      _mesh->getScene()->onBeforeRenderObservable.remove(
        onBeforeRenderObserver);
      _axesViewer->dispose();
      _axesViewer = nullptr;
    }
  }
}

string_t MeshAdapter::getInfo() const
{
  if (_mesh) {
    ::std::to_string(_mesh->getTotalVertices()) + " vertices";
  }

  return "0 vertices";
}

void MeshAdapter::_drawAxis()
{
  _obj->computeWorldMatrix();

  _axesViewer = ::std::make_unique<Debug::AxesViewer>(_obj->getScene());

  onBeforeRenderObserver = _obj->getScene()->onBeforeRenderObservable.add(
    [&](Scene* /*scene*/, EventState& /*es*/) {
      if (auto mesh = dynamic_cast<TransformNode*>(_obj)) {
        auto matrix = *mesh->getWorldMatrix();
        Vector3 extend(1.f, 1.f, 1.f);
        if (_mesh) {
          extend = _mesh->getBoundingInfo().boundingBox.extendSizeWorld;
        }
        if (_axesViewer) {
          _axesViewer->scaleLines
            = ::std::max(::std::max(extend.x, extend.y), extend.z) * 2.f;
          _axesViewer->update(mesh->position(),
                              Vector3::TransformNormal(_x, matrix),
                              Vector3::TransformNormal(_y, matrix),
                              Vector3::TransformNormal(_z, matrix));
        }
      }
    });
}

} // end of namespace BABYLON
