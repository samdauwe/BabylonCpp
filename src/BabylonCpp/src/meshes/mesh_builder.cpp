#include <babylon/meshes/mesh_builder.h>

#include <babylon/cameras/camera.h>
#include <babylon/core/logging.h>
#include <babylon/culling/bounding_info.h>
#include <babylon/engines/scene.h>
#include <babylon/math/axis.h>
#include <babylon/math/path3d.h>
#include <babylon/math/position_normal_vertex.h>
#include <babylon/math/tmp.h>
#include <babylon/meshes/_creation_data_storage.h>
#include <babylon/meshes/builders/box_builder.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/decal_builder.h>
#include <babylon/meshes/builders/disc_builder.h>
#include <babylon/meshes/builders/ico_sphere_builder.h>
#include <babylon/meshes/builders/lathe_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/ribbon_builder.h>
#include <babylon/meshes/builders/sphere_builder.h>
#include <babylon/meshes/builders/tube_builder.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/polygonmesh/polygon_mesh_builder.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/meshes/vertex_data_options.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

MeshPtr MeshBuilder::CreateBox(const std::string& name, BoxOptions& options,
                               Scene* scene)
{
  return BoxBuilder::CreateBox(name, options, scene);
}

MeshPtr MeshBuilder::CreateSphere(const std::string& name,
                                  SphereOptions& options, Scene* scene)
{
  return SphereBuilder::CreateSphere(name, options, scene);
}

MeshPtr MeshBuilder::CreateDisc(const std::string& name, DiscOptions& options,
                                Scene* scene)
{
  return DiscBuilder::CreateDisc(name, options, scene);
}

MeshPtr MeshBuilder::CreateIcoSphere(const std::string& name,
                                     IcoSphereOptions& options, Scene* scene)
{
  return IcoSphereBuilder::CreateIcoSphere(name, options, scene);
}

MeshPtr MeshBuilder::CreateRibbon(const std::string& name,
                                  RibbonOptions& options, Scene* scene)

{
  return RibbonBuilder::CreateRibbon(name, options, scene);
}

MeshPtr MeshBuilder::CreateCylinder(const std::string& name,
                                    CylinderOptions& options, Scene* scene)
{
  return CylinderBuilder::CreateCylinder(name, options, scene);
}

MeshPtr MeshBuilder::CreateTorus(const std::string& name, TorusOptions& options,
                                 Scene* scene)
{
  auto torus = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  torus->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateTorus(options);

  vertexData->applyToMesh(*torus, options.updatable);

  return torus;
}

MeshPtr MeshBuilder::CreateTorusKnot(const std::string& name,
                                     TorusKnotOptions& options, Scene* scene)
{
  auto torusKnot = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  torusKnot->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateTorusKnot(options);

  vertexData->applyToMesh(*torusKnot, options.updatable);

  return torusKnot;
}

LinesMeshPtr MeshBuilder::CreateLineSystem(const std::string& name,
                                           LineSystemOptions& options,
                                           Scene* scene)
{
  const auto& instance = options.instance;
  const auto& lines    = options.lines;
  const auto& colors   = options.colors;

  if (instance) { // lines update
    auto positions = instance->getVerticesData(VertexBuffer::PositionKind);
    Float32Array vertexColor;
    std::vector<Color4> lineColors;
    if (!colors.empty()) {
      vertexColor = instance->getVerticesData(VertexBuffer::ColorKind);
    }
    size_t i = 0;
    size_t c = 0;
    for (size_t l = 0; l < lines.size(); ++l) {
      const auto& points = lines[l];
      for (size_t p = 0; p < points.size(); p++) {
        positions[i]     = points[p].x;
        positions[i + 1] = points[p].y;
        positions[i + 2] = points[p].z;
        if (!colors.empty() && !vertexColor.empty()) {
          lineColors         = colors[l];
          vertexColor[c]     = lineColors[p].r;
          vertexColor[c + 1] = lineColors[p].g;
          vertexColor[c + 2] = lineColors[p].b;
          vertexColor[c + 3] = lineColors[p].a;
          c += 4;
        }
        i += 3;
      }
    }
    instance->updateVerticesData(VertexBuffer::PositionKind, positions, false,
                                 false);
    if (!colors.empty() && !vertexColor.empty()) {
      instance->updateVerticesData(VertexBuffer::ColorKind, vertexColor, false,
                                   false);
    }
    return instance;
  }

  // line system creation
  auto useVertexColor = (!colors.empty()) ? true : false;
  auto lineSystem     = LinesMesh::New(name, scene, nullptr, nullptr, true,
                                   useVertexColor, options.useVertexAlpha);
  auto vertexData     = VertexData::CreateLineSystem(options);
  vertexData->applyToMesh(*lineSystem, options.updatable);
  return lineSystem;
}

LinesMeshPtr MeshBuilder::CreateLines(const std::string& name,
                                      LinesOptions& options, Scene* scene)
{
  LineSystemOptions lineSystemOptions(options);
  return MeshBuilder::CreateLineSystem(name, lineSystemOptions, scene);
}

LinesMeshPtr MeshBuilder::CreateDashedLines(const std::string& name,
                                            DashedLinesOptions& options,
                                            Scene* scene)
{
  const auto& points   = options.points;
  const auto& instance = options.instance;
  const auto& gapSize  = options.gapSize;
  const auto& dashSize = options.dashSize;

  if (instance) { //  dashed lines update
    const auto positionFunction = [&](Float32Array& positions) -> void {
      auto curvect    = Vector3::Zero();
      auto nbSeg      = positions.size() / 6;
      auto lg         = 0.f;
      unsigned int nb = 0;
      auto shft       = 0.f;
      auto dashshft   = 0.f;
      auto curshft    = 0.f;
      unsigned int p  = 0;
      unsigned int j  = 0;
      for (std::size_t i = 0; i < points.size() - 1; ++i) {
        points[i + 1].subtractToRef(points[i], curvect);
        lg += curvect.length();
      }
      shft          = lg / static_cast<float>(nbSeg);
      auto dashSize = instance->_creationDataStorage->dashSize;
      auto gapSize  = instance->_creationDataStorage->gapSize;
      dashshft      = dashSize * shft / static_cast<float>(dashSize + gapSize);
      std::size_t i = 0;
      for (i = 0; i < points.size() - 1; ++i) {
        points[i + 1].subtractToRef(points[i], curvect);
        nb = static_cast<unsigned int>(std::floor(curvect.length() / shft));
        curvect.normalize();
        j = 0;
        while (j < nb && p < positions.size()) {
          curshft          = shft * static_cast<float>(j);
          positions[p]     = points[i].x + curshft * curvect.x;
          positions[p + 1] = points[i].y + curshft * curvect.y;
          positions[p + 2] = points[i].z + curshft * curvect.z;
          positions[p + 3] = points[i].x + (curshft + dashshft) * curvect.x;
          positions[p + 4] = points[i].y + (curshft + dashshft) * curvect.y;
          positions[p + 5] = points[i].z + (curshft + dashshft) * curvect.z;
          p += 6;
          ++j;
        }
      }
      while (p < positions.size()) {
        positions[p]     = points[i].x;
        positions[p + 1] = points[i].y;
        positions[p + 2] = points[i].z;
        p += 3;
      }
    };
    instance->updateMeshPositions(positionFunction, false);
    return instance;
  }
  // dashed lines creation
  auto dashedLines = LinesMesh::New(name, scene);
  auto vertexData  = VertexData::CreateDashedLines(options);
  vertexData->applyToMesh(*dashedLines, options.updatable);
  dashedLines->_creationDataStorage = std::make_shared<_CreationDataStorage>();
  dashedLines->_creationDataStorage->dashSize = dashSize;
  dashedLines->_creationDataStorage->gapSize  = gapSize;
  return dashedLines;
}

MeshPtr MeshBuilder::ExtrudeShape(const std::string& name,
                                  ExtrudeShapeOptions& options, Scene* scene)
{
  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);

  return MeshBuilder::_ExtrudeShapeGeneric(
    name, options.shape, options.path, options.scale, options.rotation, nullptr,
    nullptr, false, false, options.cap, false, scene, options.updatable,
    options.sideOrientation, options.instance, options.invertUV,
    options.frontUVs, options.backUVs);
}

MeshPtr MeshBuilder::ExtrudeShapeCustom(const std::string& name,
                                        ExtrudeShapeCustomOptions& options,
                                        Scene* scene)
{
  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);

  return MeshBuilder::_ExtrudeShapeGeneric(
    name, options.shape, options.path, 0.f, 0.f, options.scaleFunction,
    options.rotationFunction, options.ribbonCloseArray, options.ribbonClosePath,
    options.cap, true, scene, options.updatable, options.sideOrientation,
    options.instance, options.invertUV, options.frontUVs, options.backUVs);
}

MeshPtr MeshBuilder::CreateLathe(const std::string& name, LatheOptions& options,
                                 Scene* scene)
{
  return LatheBuilder::CreateLathe(name, options, scene);
}

MeshPtr MeshBuilder::CreatePlane(const std::string& name, PlaneOptions& options,
                                 Scene* scene)
{
  auto plane = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  plane->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreatePlane(options);

  vertexData->applyToMesh(*plane, options.updatable);

  if (options.sourcePlane) {
    plane->translate(options.sourcePlane->normal, options.sourcePlane->d);

    auto product
      = std::acos(Vector3::Dot(options.sourcePlane->normal, Axis::Z()));
    auto vectorProduct = Vector3::Cross(Axis::Z(), options.sourcePlane->normal);

    if (vectorProduct.lengthSquared() > Math::Epsilon) {
      plane->rotate(vectorProduct, product);
    }
  }

  return plane;
}

MeshPtr MeshBuilder::CreateGround(const std::string& name,
                                  GroundOptions& options, Scene* scene)
{
  auto ground = GroundMesh::New(name, scene);
  ground->_setReady(false);
  ground->_subdivisionsX = options.subdivisionsX;
  ground->_subdivisionsY = options.subdivisionsY;
  ground->_width         = static_cast<float>(options.width);
  ground->_height        = static_cast<float>(options.height);
  ground->_maxX          = ground->_width / 2.f;
  ground->_maxZ          = ground->_height / 2.f;
  ground->_minX          = -ground->_maxX;
  ground->_minZ          = -ground->_maxZ;

  auto vertexData = VertexData::CreateGround(options);

  vertexData->applyToMesh(*ground, options.updatable);

  ground->_setReady(true);

  return ground;
}

MeshPtr MeshBuilder::CreateTiledGround(const std::string& name,
                                       TiledGroundOptions& options,
                                       Scene* scene)
{
  auto tiledGround = Mesh::New(name, scene);

  auto vertexData = VertexData::CreateTiledGround(options);

  vertexData->applyToMesh(*tiledGround, options.updatable);

  return tiledGround;
}

GroundMeshPtr MeshBuilder::CreateGroundFromHeightMap(
  const std::string& name, const std::string& url,
  GroundFromHeightMapOptions& options, Scene* scene)
{
  const auto& filter = options.colorFilter;

  auto ground            = GroundMesh::New(name, scene);
  ground->_subdivisionsX = options.subdivisions;
  ground->_subdivisionsY = options.subdivisions;
  ground->_width         = options.width;
  ground->_height        = options.height;
  ground->_maxX          = ground->_width / 2.f;
  ground->_maxZ          = ground->_height / 2.f;
  ground->_minX          = -ground->_maxX;
  ground->_minZ          = -ground->_maxZ;

  ground->_setReady(false);

  const auto onload = [&](const Image& img) {
    if (scene->isDisposed()) {
      return;
    }

    // Create VertexData from map data
    options.bufferWidth  = static_cast<unsigned int>(img.width);
    options.bufferHeight = static_cast<unsigned int>(img.height);
    options.buffer       = img.data;
    options.colorFilter  = filter;

    auto vertexData = VertexData::CreateGroundFromHeightMap(options);

    vertexData->applyToMesh(*ground, options.updatable);

    // execute ready callback, if set
    if (options.onReady) {
      options.onReady(ground.get());
    }

    ground->_setReady(true);
  };

  const auto onError
    = [](const std::string& msg, const std::string& /*exception*/) {
        BABYLON_LOG_ERROR("Tools", msg)
      };

  Tools::LoadImageFromUrl(url, onload, onError, false);

  return ground;
}

MeshPtr MeshBuilder::CreatePolygon(const std::string& name,
                                   PolygonOptions& options, Scene* scene)
{
  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  const auto& shape = options.shape;
  const auto& holes = options.holes;
  const auto& depth = options.depth;
  std::vector<Vector2> contours(shape.size());
  std::vector<Vector2> hole;
  for (std::size_t i = 0; i < shape.size(); ++i) {
    contours[i] = Vector2(shape[i].x, shape[i].z);
  }
  float epsilon = 0.00000001f;
  if (contours[0].equalsWithEpsilon(contours.back(), epsilon)) {
    contours.pop_back();
  }

  PolygonMeshBuilder polygonTriangulation(name, contours, scene);
  for (std::size_t hNb = 0; hNb < holes.size(); ++hNb) {
    hole.clear();
    for (std::size_t hPoint = 0; hPoint < holes[hNb].size(); ++hPoint) {
      hole.emplace_back(Vector2(holes[hNb][hPoint].x, holes[hNb][hPoint].z));
    }
    polygonTriangulation.addHole(hole);
  }
  auto polygon = polygonTriangulation.build(options.updatable, depth);
  polygon->_originalBuilderSideOrientation = options.sideOrientation;
  auto vertexData                          = VertexData::CreatePolygon(
    polygon.get(), options.sideOrientation, options.faceUV, options.faceColors,
    options.frontUVs, options.backUVs);
  vertexData->applyToMesh(*polygon, options.updatable);

  return polygon;
}

MeshPtr MeshBuilder::ExtrudePolygon(const std::string& name,
                                    PolygonOptions& options, Scene* scene)
{
  return MeshBuilder::CreatePolygon(name, options, scene);
}

MeshPtr MeshBuilder::CreateTube(const std::string& name, TubeOptions& options,
                                Scene* scene)
{
  return TubeBuilder::CreateTube(name, options, scene);
}

MeshPtr MeshBuilder::CreatePolyhedron(const std::string& name,
                                      PolyhedronOptions& options, Scene* scene)
{
  auto polyhedron = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  polyhedron->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreatePolyhedron(options);

  vertexData->applyToMesh(*polyhedron, options.updatable);

  return polyhedron;
}

MeshPtr MeshBuilder::CreateDecal(const std::string& name,
                                 const AbstractMeshPtr& sourceMesh,
                                 DecalOptions& options)
{
  return DecalBuilder::CreateDecal(name, sourceMesh, options);
}

MeshPtr MeshBuilder::_ExtrudeShapeGeneric(
  const std::string& name, const std::vector<Vector3>& shape,
  const std::vector<Vector3>& curve, const std::optional<float>& scale,
  const std::optional<float>& rotation,
  const std::function<float(float i, float distance)>& scaleFunction,
  const std::function<float(float i, float distance)>& rotateFunction,
  bool rbCA, bool rbCP, unsigned int cap, bool custom, Scene* scene,
  bool updtbl, unsigned int side, MeshPtr instance, bool invertUV,
  const std::optional<Vector4>& frontUVs, const std::optional<Vector4>& backUVs)
{
  // extrusion geometry
  const auto extrusionPathArray
    = [](const std::vector<Vector3>& _shape, const std::vector<Vector3>& _curve,
         Path3D& path3D, std::vector<std::vector<Vector3>> shapePaths,
         const std::optional<float>& _scale,
         const std::optional<float>& _rotation,
         const std::function<float(float i, float distance)>& _scaleFunction,
         const std::function<float(float i, float distance)>& _rotateFunction,
         unsigned int _cap, bool _custom) {
        auto& tangents        = path3D.getTangents();
        const auto& normals   = path3D.getNormals();
        const auto& binormals = path3D.getBinormals();
        const auto& distances = path3D.getDistances();

        auto angle       = 0.f;
        auto returnScale = [_scale](float /*i*/, float /*distance*/) {
          return _scale.has_value() ? *_scale : 1.f;
        };
        auto returnRotation = [_rotation](float /*i*/, float /*distance*/) {
          return _rotation.has_value() ? *_rotation : 0.f;
        };
        auto rotate = _custom ? _rotateFunction : returnRotation;
        auto scl    = _custom ? _scaleFunction : returnScale;
        unsigned int index
          = (_cap == Mesh::NO_CAP || _cap == Mesh::CAP_END) ? 0 : 2;
        auto& rotationMatrix = Tmp::MatrixArray[0];
        shapePaths.resize(_curve.size());

        for (std::size_t i = 0; i < _curve.size(); ++i) {
          std::vector<Vector3> shapePath;
          auto angleStep  = rotate(static_cast<float>(i), distances[i]);
          auto scaleRatio = scl(static_cast<float>(i), distances[i]);
          for (std::size_t p = 0; p < _shape.size(); ++p) {
            Matrix::RotationAxisToRef(tangents[i], angle, rotationMatrix);
            auto planed  = ((tangents[i].scale(_shape[p].z))
                             .add(normals[i].scale(_shape[p].x))
                             .add(binormals[i].scale(_shape[p].y)));
            auto rotated = Vector3::Zero();
            Vector3::TransformCoordinatesToRef(planed, rotationMatrix, rotated);
            rotated.scaleInPlace(scaleRatio).addInPlace(_curve[i]);
            shapePath.emplace_back(rotated);
          }
          shapePaths[index] = shapePath;
          angle += angleStep;
          ++index;
        }
        // cap
        const auto capPath = [&](const std::vector<Vector3>& shapePath) {
          std::vector<Vector3> pointCap;
          auto barycenter = Vector3::Zero();
          for (std::size_t i = 0; i < shapePath.size(); ++i) {
            barycenter.addInPlace(shapePath[i]);
          }
          barycenter.scaleInPlace(1.f / static_cast<float>(shapePath.size()));
          for (std::size_t i = 0; i < shapePath.size(); ++i) {
            pointCap.emplace_back(barycenter);
          }
          return pointCap;
        };
        switch (_cap) {
          case Mesh::NO_CAP:
            break;
          case Mesh::CAP_START:
            shapePaths[0] = capPath(shapePaths[2]);
            shapePaths[1] = shapePaths[2];
            break;
          case Mesh::CAP_END:
            shapePaths[index + 0] = shapePaths[index - 1];
            shapePaths[index + 1] = capPath(shapePaths[index - 1]);
            break;
          case Mesh::CAP_ALL:
            shapePaths[0]         = capPath(shapePaths[2]);
            shapePaths[1]         = shapePaths[2];
            shapePaths[index + 0] = shapePaths[index - 1];
            shapePaths[index + 1] = capPath(shapePaths[index - 1]);
            break;
          default:
            break;
        }
        return shapePaths;
      };

  Path3D path3D;
  std::vector<std::vector<Vector3>> pathArray;
  if (instance) { // instance update
    auto& storage = instance->_creationDataStorage;
    path3D        = storage->path3D.update(curve);
    pathArray     = extrusionPathArray(
      shape, curve, storage->path3D, storage->pathArray, scale, rotation,
      scaleFunction, rotateFunction, storage->cap, custom);
    instance = Mesh::CreateRibbon("", pathArray, false, false, 0, scene, false,
                                  Mesh::DEFAULTSIDE, instance);

    return instance;
  }
  // extruded shape creation
  path3D = Path3D(curve);
  std::vector<std::vector<Vector3>> newShapePaths;
  unsigned int _cap = (cap > 3) ? 0 : cap;
  pathArray
    = extrusionPathArray(shape, curve, path3D, newShapePaths, scale, rotation,
                         scaleFunction, rotateFunction, _cap, custom);
  RibbonOptions ribbonOptions;
  ribbonOptions.pathArray       = pathArray;
  ribbonOptions.closeArray      = rbCA;
  ribbonOptions.closePath       = rbCP;
  ribbonOptions.updatable       = updtbl;
  ribbonOptions.sideOrientation = side;
  ribbonOptions.invertUV        = invertUV;
  ribbonOptions.frontUVs        = frontUVs;
  ribbonOptions.backUVs         = backUVs;
  auto extrudedGeneric = MeshBuilder::CreateRibbon(name, ribbonOptions, scene);
  extrudedGeneric->_creationDataStorage->pathArray = pathArray;
  extrudedGeneric->_creationDataStorage->path3D    = path3D;
  extrudedGeneric->_creationDataStorage->cap       = _cap;

  return extrudedGeneric;
}

unsigned int MeshBuilder::updateSideOrientation(unsigned int orientation)
{
  if (orientation == Mesh::DOUBLESIDE) {
    return Mesh::DOUBLESIDE;
  }

  if (orientation == Mesh::DEFAULTSIDE) {
    return Mesh::FRONTSIDE;
  }

  return orientation;
}

} // end of namespace BABYLON
