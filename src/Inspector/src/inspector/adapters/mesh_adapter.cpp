#include <babylon/inspector/adapters/mesh_adapter.h>

#include <babylon/debug/axes_viewer.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>
#include <babylon/mesh/ground_mesh.h>
#include <babylon/mesh/mesh.h>

#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/bounding_box.h>
#include <babylon/inspector/treetools/checkbox.h>
#include <babylon/inspector/treetools/debug_area.h>
#include <babylon/inspector/treetools/info.h>

namespace BABYLON {

MeshAdapter::MeshAdapter(const NodePtr& obj)
    : _obj{obj}
    , _abstractMesh{nullptr}
    , _mesh{nullptr}
    , _groundMesh{nullptr}
    , _properties{nullptr}
    , _axesViewer{nullptr}
    , onBeforeRenderObserver{nullptr}
    , _x{1.f, 0.f, 0.f}
    , _y{0.f, 1.f, 0.f}
    , _z{0.f, 0.f, 1.f}
{
  _tools.emplace_back(std::make_unique<Checkbox>(this));
  _tools.emplace_back(std::make_unique<DebugArea>(this));
  // Cast mesh
  _abstractMesh = std::static_pointer_cast<AbstractMesh>(obj);
  if (_abstractMesh) {
    // Mesh
    if (_abstractMesh->type() == IReflect::Type::MESH) {
      _mesh = std::static_pointer_cast<Mesh>(obj);
    }
    // GroundMesh
    if (_abstractMesh->type() == IReflect::Type::GROUNDMESH) {
      _mesh       = std::static_pointer_cast<Mesh>(obj);
      _groundMesh = std::static_pointer_cast<GroundMesh>(obj);
    }
    // Bounding box
    if (_abstractMesh->getTotalVertices() > 0) {
      _tools.emplace_back(std::make_unique<BoundingBoxInsp>(this));
    }
  }
  _tools.emplace_back(std::make_unique<Info>(this));
  // Build properties view
  if (_abstractMesh) {
    _properties = std::make_unique<PropertiesView>();
    _buildPropertiesView();
  }
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

unique_ptr_t<PropertiesView>& MeshAdapter::getProperties()
{
  return _properties;
}

vector_t<unique_ptr_t<AbstractTreeTool>>& MeshAdapter::getTools()
{
  return _tools;
}

void MeshAdapter::setVisible(bool b)
{
  _obj->setEnabled(b);
  if (_abstractMesh) {
    _abstractMesh->isVisible = b;
  }
}

bool MeshAdapter::isVisible() const
{
  return _obj->isEnabled() && (_abstractMesh && _abstractMesh->isVisible);
}

bool MeshAdapter::isBoxVisible() const
{
#if 0
  return _abstractMesh ? _abstractMesh->showBoundingBox : false;
#else
  return false;
#endif
}

void MeshAdapter::setBoxVisible(bool /*b*/)
{
  if (_abstractMesh) {
#if 0
    _abstractMesh->showBoundingBox = b;
#endif
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
    if (_abstractMesh) {
      _abstractMesh->getScene()->onBeforeRenderObservable.remove(
        onBeforeRenderObserver);
      _axesViewer->dispose();
      _axesViewer = nullptr;
    }
  }
}

string_t MeshAdapter::getInfo() const
{
  if (_abstractMesh) {
    std::to_string(_abstractMesh->getTotalVertices()) + " vertices";
  }

  return "0 vertices";
}

void MeshAdapter::_drawAxis()
{
  _obj->computeWorldMatrix();

  _axesViewer = std::make_unique<Debug::AxesViewer>(_obj->getScene());

  onBeforeRenderObserver = _obj->getScene()->onBeforeRenderObservable.add(
    [&](Scene* /*scene*/, EventState& /*es*/) {
      if (auto mesh = std::static_pointer_cast<TransformNode>(_obj)) {
        auto matrix = *mesh->getWorldMatrix();
        Vector3 extend(1.f, 1.f, 1.f);
        if (_abstractMesh) {
          extend = _abstractMesh->getBoundingInfo().boundingBox.extendSizeWorld;
        }
        if (_axesViewer) {
          _axesViewer->scaleLines
            = std::max(std::max(extend.x, extend.y), extend.z) * 2.f;
          _axesViewer->update(mesh->position(),
                              Vector3::TransformNormal(_x, matrix),
                              Vector3::TransformNormal(_y, matrix),
                              Vector3::TransformNormal(_z, matrix));
        }
      }
    });
}

void MeshAdapter::_buildPropertiesView()
{
  // -- Add properties -- //
  auto& view = *_properties;
  /** Node properties **/
  // - id
  view.addStringProperty(
    "id", [&]() -> string_t& { return _abstractMesh->id; },
    [&](const string_t& value) { _abstractMesh->id = value; });
  // - uniqueId
  view.addSizeTProperty(
    "uniqueId", [&]() -> size_t { return _abstractMesh->uniqueId; },
    [&](const size_t& value) { _abstractMesh->uniqueId = value; });
  // - name
  view.addStringProperty(
    "name", [&]() -> string_t& { return _abstractMesh->name; },
    [&](const string_t& value) { _abstractMesh->name = value; });
  // - doNotSerialize
  view.addBoolProperty(
    "doNotSerialize", [&]() -> bool { return _abstractMesh->doNotSerialize; },
    [&](const bool& value) { _abstractMesh->doNotSerialize = value; });
  /** TransformNode properties **/
  // - absolutePosition
  view.addVector3Property(
    "absolutePosition",
    [&]() -> Vector3 const& { return _abstractMesh->absolutePosition(); },
    [&](const Vector3& value) { _abstractMesh->setAbsolutePosition(value); });
  // - billboardMode
  view.addSizeTProperty(
    "billboardMode", [&]() -> size_t { return _abstractMesh->billboardMode; },
    [&](const size_t& value) {
      _abstractMesh->billboardMode = static_cast<unsigned>(value);
    });
  // - infiniteDistance
  view.addBoolProperty(
    "infiniteDistance",
    [&]() -> bool { return _abstractMesh->infiniteDistance; },
    [&](const bool& value) { _abstractMesh->infiniteDistance = value; });
  // - isWorldMatrixFrozen
  view.addBoolProperty(
    "isWorldMatrixFrozen",
    [&]() -> bool { return _abstractMesh->isWorldMatrixFrozen(); },
    [&](const bool& /*value*/) {});
  // - nonUniformScaling
  view.addBoolProperty(
    "nonUniformScaling",
    [&]() -> bool { return _abstractMesh->nonUniformScaling(); },
    [&](const bool& /*value*/) {});
  // - position
  view.addVector3Property(
    "position", [&]() -> Vector3 const& { return _abstractMesh->position(); },
    [&](const Vector3& value) { _abstractMesh->position = value; });
  // - rotation
  view.addVector3Property(
    "rotation", [&]() -> Vector3 const& { return _abstractMesh->rotation(); },
    [&](const Vector3& value) { _abstractMesh->rotation = value; });
  // - scaling
  view.addVector3Property(
    "scaling", [&]() -> Vector3 const& { return _abstractMesh->scaling(); },
    [&](const Vector3& value) { _abstractMesh->scaling = value; });
  // - scalingDeterminant
  view.addFloatProperty(
    "scalingDeterminant",
    [&]() -> float { return _abstractMesh->scalingDeterminant; },
    [&](const float& value) { _abstractMesh->scalingDeterminant = value; });
  /** AbstractMesh properties **/
  // - alphaIndex
  view.addIntProperty(
    "alphaIndex", [&]() -> int { return _abstractMesh->alphaIndex; },
    [&](const int& value) { _abstractMesh->alphaIndex = value; });
  // - alwaysSelectAsActiveMesh
  view.addBoolProperty(
    "alwaysSelectAsActiveMesh",
    [&]() -> bool { return _abstractMesh->alwaysSelectAsActiveMesh; },
    [&](const bool& value) {
      _abstractMesh->alwaysSelectAsActiveMesh = value;
    });
  // - applyFog
  view.addBoolProperty(
    "applyFog", [&]() -> bool { return _abstractMesh->applyFog(); },
    [&](const bool& value) { _abstractMesh->applyFog = value; });
  // - checkCollisions
  view.addBoolProperty(
    "checkCollisions",
    [&]() -> bool { return _abstractMesh->checkCollisions(); },
    [&](const bool& value) { _abstractMesh->checkCollisions = value; });
  // - collisionGroup
  view.addIntProperty(
    "collisionGroup", [&]() -> int { return _abstractMesh->collisionGroup(); },
    [&](const int& value) { _abstractMesh->collisionGroup = value; });
  // - collisionMask
  view.addIntProperty(
    "collisionMask", [&]() -> int { return _abstractMesh->collisionMask(); },
    [&](const int& value) { _abstractMesh->collisionMask = value; });
  // - computeBonesUsingShaders
  view.addBoolProperty(
    "computeBonesUsingShaders",
    [&]() -> bool { return _abstractMesh->computeBonesUsingShaders(); },
    [&](const bool& value) {
      _abstractMesh->computeBonesUsingShaders = value;
    });
  // - definedFacingForward
  view.addBoolProperty(
    "definedFacingForward",
    [&]() -> bool { return _abstractMesh->definedFacingForward; },
    [&](const bool& value) { _abstractMesh->definedFacingForward = value; });
  // - edgesColor
  view.addColor4Property(
    "edgesColor", [&]() -> Color4 const& { return _abstractMesh->edgesColor; },
    [&](const Color4& value) { _abstractMesh->edgesColor = value; });
  // - edgesWidth
  view.addFloatProperty(
    "edgesWidth", [&]() -> float { return _abstractMesh->edgesWidth; },
    [&](const float& value) { _abstractMesh->edgesWidth = value; });
  // - ellipsoid
  view.addVector3Property(
    "ellipsoid", [&]() -> Vector3 const& { return _abstractMesh->ellipsoid; },
    [&](const Vector3& value) { _abstractMesh->ellipsoid = value; });
  // - ellipsoidOffset
  view.addVector3Property(
    "ellipsoidOffset",
    [&]() -> Vector3 const& { return _abstractMesh->ellipsoidOffset; },
    [&](const Vector3& value) { _abstractMesh->ellipsoidOffset = value; });
  // - enablePointerMoveEvents
  view.addBoolProperty(
    "enablePointerMoveEvents",
    [&]() -> bool { return _abstractMesh->enablePointerMoveEvents; },
    [&](const bool& value) { _abstractMesh->enablePointerMoveEvents = value; });
  // - facetNb
  view.addSizeTProperty("facetNb",
                        [&]() -> size_t { return _abstractMesh->facetNb(); },
                        [&](const size_t& /*value*/) {});
  // - hasVertexAlpha
  view.addBoolProperty(
    "hasVertexAlpha", [&]() -> bool { return _abstractMesh->hasVertexAlpha(); },
    [&](const bool& value) { _abstractMesh->hasVertexAlpha = value; });
  // - isBlocked
  view.addBoolProperty("isBlocked",
                       [&]() -> bool { return _abstractMesh->isBlocked(); },
                       [&](const bool& /*value*/) {});
  // - isBlocker
  view.addBoolProperty(
    "isBlocker", [&]() -> bool { return _abstractMesh->isBlocker; },
    [&](const bool& value) { _abstractMesh->isBlocker = value; });
  // - isFacetDataEnabled
  view.addBoolProperty(
    "isFacetDataEnabled",
    [&]() -> bool { return _abstractMesh->isFacetDataEnabled(); },
    [&](const bool& /*value*/) {});
  // - isOccluded
  view.addBoolProperty("isOccluded",
                       [&]() -> bool { return _abstractMesh->isOccluded(); },
                       [&](const bool& /*value*/) {});
  // - isOcclusionQueryInProgress
  view.addBoolProperty(
    "isOcclusionQueryInProgress",
    [&]() -> bool { return _abstractMesh->isOcclusionQueryInProgress(); },
    [&](const bool& /*value*/) {});
  // - isPickable
  view.addBoolProperty(
    "isPickable", [&]() -> bool { return _abstractMesh->isPickable; },
    [&](const bool& value) { _abstractMesh->isPickable = value; });
  // - isVisible
  view.addBoolProperty(
    "isVisible", [&]() -> bool { return _abstractMesh->isVisible; },
    [&](const bool& value) { _abstractMesh->isVisible = value; });
  // - layerMask
  view.addSizeTProperty(
    "layerMask", [&]() -> size_t { return _abstractMesh->layerMask(); },
    [&](const size_t& value) {
      _abstractMesh->layerMask = static_cast<unsigned>(value);
    });
  // - mustDepthSortFacets
  view.addBoolProperty(
    "mustDepthSortFacets",
    [&]() -> bool { return _abstractMesh->mustDepthSortFacets(); },
    [&](const bool& value) { _abstractMesh->mustDepthSortFacets = value; });
  // - numBoneInfluencers
  view.addSizeTProperty(
    "numBoneInfluencers",
    [&]() -> size_t { return _abstractMesh->numBoneInfluencers(); },
    [&](const size_t& value) {
      _abstractMesh->numBoneInfluencers = static_cast<unsigned>(value);
    });
  // - occlusionQueryAlgorithmType
  view.addSizeTProperty(
    "occlusionQueryAlgorithmType",
    [&]() -> size_t { return _abstractMesh->occlusionQueryAlgorithmType; },
    [&](const size_t& value) {
      _abstractMesh->occlusionQueryAlgorithmType = static_cast<unsigned>(value);
    });
  // - occlusionRetryCount
  view.addIntProperty(
    "occlusionRetryCount",
    [&]() -> int { return _abstractMesh->occlusionRetryCount; },
    [&](const int& value) { _abstractMesh->occlusionRetryCount = value; });
  // - occlusionType
  view.addSizeTProperty(
    "occlusionType", [&]() -> size_t { return _abstractMesh->occlusionType; },
    [&](const size_t& value) {
      _abstractMesh->occlusionType = static_cast<unsigned>(value);
    });
  // - outlineColor
  view.addColor3Property(
    "outlineColor",
    [&]() -> Color3 const& { return _abstractMesh->outlineColor; },
    [&](const Color3& value) { _abstractMesh->outlineColor = value; });
  // - outlineWidth
  view.addFloatProperty(
    "outlineWidth", [&]() -> float { return _abstractMesh->outlineWidth; },
    [&](const float& value) { _abstractMesh->outlineWidth = value; });
  // - overlayAlpha
  view.addFloatProperty(
    "overlayAlpha", [&]() -> float { return _abstractMesh->overlayAlpha; },
    [&](const float& value) { _abstractMesh->overlayAlpha = value; });
  // - overlayColor
  view.addColor3Property(
    "overlayColor",
    [&]() -> Color3 const& { return _abstractMesh->overlayColor; },
    [&](const Color3& value) { _abstractMesh->overlayColor = value; });
  // - partitioningBBoxRatio
  view.addFloatProperty(
    "partitioningBBoxRatio",
    [&]() -> float { return _abstractMesh->partitioningBBoxRatio(); },
    [&](const float& value) { _abstractMesh->partitioningBBoxRatio = value; });
  // - partitioningSubdivisions
  view.addSizeTProperty(
    "partitioningSubdivisions",
    [&]() -> size_t { return _abstractMesh->partitioningSubdivisions(); },
    [&](const size_t& value) {
      _abstractMesh->partitioningSubdivisions = static_cast<unsigned>(value);
    });
  // - receiveShadows
  view.addBoolProperty(
    "receiveShadows", [&]() -> bool { return _abstractMesh->receiveShadows(); },
    [&](const bool& value) { _abstractMesh->receiveShadows = value; });
  // - renderingGroupId
  view.addSizeTProperty(
    "renderingGroupId",
    [&]() -> size_t { return _abstractMesh->renderingGroupId; },
    [&](const size_t& value) {
      _abstractMesh->renderingGroupId = static_cast<unsigned>(value);
    });
  // - renderOutline
  view.addBoolProperty(
    "renderOutline", [&]() -> bool { return _abstractMesh->renderOutline; },
    [&](const bool& value) { _abstractMesh->renderOutline = value; });
  // - renderOverlay
  view.addBoolProperty(
    "renderOverlay", [&]() -> bool { return _abstractMesh->renderOverlay; },
    [&](const bool& value) { _abstractMesh->renderOverlay = value; });
  // - showBoundingBox
#if 0
  view.addBoolProperty(
    "showBoundingBox", [&]() -> bool { return _abstractMesh->showBoundingBox; },
    [&](const bool& value) { _abstractMesh->showBoundingBox = value; });
#endif
  // - showSubMeshesBoundingBox
  view.addBoolProperty(
    "showSubMeshesBoundingBox",
    [&]() -> bool { return _abstractMesh->showSubMeshesBoundingBox; },
    [&](const bool& value) {
      _abstractMesh->showSubMeshesBoundingBox = value;
    });
  // - useOctreeForCollisions
  view.addBoolProperty(
    "useOctreeForCollisions",
    [&]() -> bool { return _abstractMesh->useOctreeForCollisions; },
    [&](const bool& value) { _abstractMesh->useOctreeForCollisions = value; });
  // - useOctreeForPicking
  view.addBoolProperty(
    "useOctreeForPicking",
    [&]() -> bool { return _abstractMesh->useOctreeForPicking; },
    [&](const bool& value) { _abstractMesh->useOctreeForPicking = value; });
  // - useOctreeForRenderingSelection
  view.addBoolProperty(
    "useOctreeForRenderingSelection",
    [&]() -> bool { return _abstractMesh->useOctreeForRenderingSelection; },
    [&](const bool& value) {
      _abstractMesh->useOctreeForRenderingSelection = value;
    });
  // - useVertexColors
  view.addBoolProperty(
    "useVertexColors",
    [&]() -> bool { return _abstractMesh->useVertexColors(); },
    [&](const bool& value) { _abstractMesh->useVertexColors = value; });
  // - visibility
  view.addFloatProperty(
    "visibility", [&]() -> float { return _abstractMesh->visibility(); },
    [&](const float& value) { _abstractMesh->visibility = value; });
  /** Mesh properties **/
  if (_mesh) {
    // - areNormalsFrozen
    view.addBoolProperty("areNormalsFrozen",
                         [&]() -> bool { return _mesh->areNormalsFrozen(); },
                         [&](const bool& /*value*/) {});
    // - delayLoadState
    view.addIntProperty(
      "delayLoadState", [&]() -> int { return _mesh->delayLoadState; },
      [&](const int& value) { _mesh->delayLoadState = value; });
    // - hasLODLevels
    view.addBoolProperty("hasLODLevels",
                         [&]() -> bool { return _mesh->hasLODLevels(); },
                         [&](const bool& /*value*/) {});
    // - isUnIndexed
    view.addBoolProperty(
      "isUnIndexed", [&]() -> bool { return _mesh->isUnIndexed(); },
      [&](const bool& value) { _mesh->isUnIndexed = value; });
  }
  /** GroundMesh properties **/
  if (_groundMesh) {
    // - generateOctree
    view.addBoolProperty(
      "generateOctree", [&]() -> bool { return _groundMesh->generateOctree; },
      [&](const bool& value) { _groundMesh->generateOctree = value; });
    // - subdivisions
    view.addSizeTProperty(
      "subdivisions", [&]() -> size_t { return _groundMesh->subdivisions(); },
      [&](const size_t& /*value*/) {});
    // - subdivisionsX
    view.addSizeTProperty(
      "subdivisionsX", [&]() -> size_t { return _groundMesh->subdivisionsX(); },
      [&](const size_t& /*value*/) {});
    // - subdivisionsY
    view.addSizeTProperty(
      "subdivisionsY", [&]() -> size_t { return _groundMesh->subdivisionsY(); },
      [&](const size_t& /*value*/) {});
  }
  // -- Sort properties by property name -- //
  view.sortPropertiesByName();
}

} // end of namespace BABYLON
