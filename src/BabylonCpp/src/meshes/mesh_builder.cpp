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
  auto box = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  box->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateBox(options);

  vertexData->applyToMesh(*box, options.updatable);

  return box;
}

MeshPtr MeshBuilder::CreateSphere(const std::string& name,
                                  SphereOptions& options, Scene* scene)
{
  auto sphere = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  sphere->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateSphere(options);

  vertexData->applyToMesh(*sphere, options.updatable);

  return sphere;
}

MeshPtr MeshBuilder::CreateDisc(const std::string& name, DiscOptions& options,
                                Scene* scene)
{
  auto disc = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  disc->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateDisc(options);

  vertexData->applyToMesh(*disc, options.updatable);

  return disc;
}

MeshPtr MeshBuilder::CreateIcoSphere(const std::string& name,
                                     IcoSphereOptions& options, Scene* scene)
{
  auto sphere = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  sphere->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateIcoSphere(options);

  vertexData->applyToMesh(*sphere, options.updatable);

  return sphere;
}

MeshPtr MeshBuilder::CreateRibbon(const std::string& name,
                                  RibbonOptions& options, Scene* scene)

{
  const auto& pathArray  = options.pathArray();
  const auto& closeArray = options.closeArray;
  const auto& closePath  = options.closePath;

  const auto sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  const auto& instance  = options.instance;
  const auto& updatable = options.updatable;

  if (instance) { // existing ribbon instance update
                  // positionFunction : ribbon case
    // only pathArray and sideOrientation parameters are taken into account for
    // positions update
    Vector3::FromFloatsToRef(std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max(),
                             std::numeric_limits<float>::max(),
                             Tmp::Vector3Array[0]); // minimum
    Vector3::FromFloatsToRef(std::numeric_limits<float>::lowest(),
                             std::numeric_limits<float>::lowest(),
                             std::numeric_limits<float>::lowest(),
                             Tmp::Vector3Array[1]);
    const auto positionFunction = [&](Float32Array& positions) {
      auto minlg     = pathArray[0].size();
      auto& mesh     = instance;
      unsigned int i = 0;
      unsigned int ns
        = (mesh->_originalBuilderSideOrientation == Mesh::DOUBLESIDE) ? 2 : 1;
      for (std::size_t si = 1; si <= ns; ++si) {
        for (std::size_t p = 0; p < pathArray.size(); ++p) {
          const auto& path = pathArray[p];
          const auto l     = path.size();
          minlg            = (minlg < l) ? minlg : l;
          unsigned int j   = 0;
          while (j < minlg) {
            positions[i]     = path[j].x;
            positions[i + 1] = path[j].y;
            positions[i + 2] = path[j].z;
            if (path[j].x < Tmp::Vector3Array[0].x) {
              Tmp::Vector3Array[0].x = path[j].x;
            }
            if (path[j].x > Tmp::Vector3Array[1].x) {
              Tmp::Vector3Array[1].x = path[j].x;
            }
            if (path[j].y < Tmp::Vector3Array[0].y) {
              Tmp::Vector3Array[0].y = path[j].y;
            }
            if (path[j].y > Tmp::Vector3Array[1].y) {
              Tmp::Vector3Array[1].y = path[j].y;
            }
            if (path[j].z < Tmp::Vector3Array[0].z) {
              Tmp::Vector3Array[0].z = path[j].z;
            }
            if (path[j].z > Tmp::Vector3Array[1].z) {
              Tmp::Vector3Array[1].z = path[j].z;
            }
            ++j;
            i += 3;
          }
          if (mesh->_creationDataStorage
              && mesh->_creationDataStorage->closePath) {
            positions[i + 0] = path[0].x;
            positions[i + 1] = path[0].y;
            positions[i + 2] = path[0].z;
            i += 3;
          }
        }
      }
    };
    auto positions = instance->getVerticesData(VertexBuffer::PositionKind);
    positionFunction(positions);
    instance->setBoundingInfo(
      BoundingInfo(Tmp::Vector3Array[2], Tmp::Vector3Array[3]));
    instance->getBoundingInfo().update(instance->_worldMatrix);
    instance->updateVerticesData(VertexBuffer::PositionKind, positions, false,
                                 false);
    if (!options.colors.empty()) {
      auto colors = instance->getVerticesData(VertexBuffer::ColorKind);
      for (size_t c = 0; c < options.colors.size(); ++c) {
        colors[c * 4]     = options.colors[c].r;
        colors[c * 4 + 1] = options.colors[c].g;
        colors[c * 4 + 2] = options.colors[c].b;
        colors[c * 4 + 3] = options.colors[c].a;
      }
      instance->updateVerticesData(VertexBuffer::ColorKind, colors, false,
                                   false);
    }
    if (!options.uvs.empty()) {
      auto uvs = instance->getVerticesData(VertexBuffer::UVKind);
      for (size_t i = 0; i < options.uvs.size(); ++i) {
        uvs[i * 2]     = options.uvs[i].x;
        uvs[i * 2 + 1] = options.uvs[i].y;
      }
      instance->updateVerticesData(VertexBuffer::UVKind, uvs, false, false);
    }
    if (!instance->areNormalsFrozen() || instance->isFacetDataEnabled()) {
      auto indices = instance->getIndices();
      auto normals = instance->getVerticesData(VertexBuffer::NormalKind);
      if (instance->isFacetDataEnabled()) {
        auto params = instance->getFacetDataParameters();
        VertexData::ComputeNormals(positions, indices, normals, params);
      }
      else {
        VertexData::ComputeNormals(positions, indices, normals);
      }

      if (instance->_creationDataStorage
          && instance->_creationDataStorage->closePath) {
        unsigned int indexFirst = 0;
        size_t indexLast        = 0;
        for (std::size_t p = 0; p < pathArray.size(); ++p) {
          indexFirst = instance->_creationDataStorage->idx[p] * 3;
          if (p + 1 < pathArray.size()) {
            indexLast = (instance->_creationDataStorage->idx[p + 1] - 1) * 3;
          }
          else {
            indexLast = normals.size() - 3;
          }
          normals[indexFirst]
            = (normals[indexFirst] + normals[indexLast]) * 0.5f;
          normals[indexFirst + 1]
            = (normals[indexFirst + 1] + normals[indexLast + 1]) * 0.5f;
          normals[indexFirst + 2]
            = (normals[indexFirst + 2] + normals[indexLast + 2]) * 0.5f;
          normals[indexLast]     = normals[indexFirst];
          normals[indexLast + 1] = normals[indexFirst + 1];
          normals[indexLast + 2] = normals[indexFirst + 2];
        }
      }

      if (!instance->areNormalsFrozen()) {
        instance->updateVerticesData(VertexBuffer::NormalKind, normals, false,
                                     false);
      }
    }

    return instance;
  }
  else { // new ribbon creation
    auto ribbon                             = Mesh::New(name, scene);
    ribbon->_originalBuilderSideOrientation = sideOrientation;
    ribbon->_creationDataStorage = std::make_shared<_CreationDataStorage>();

    auto vertexData = VertexData::CreateRibbon(options);
    if (closePath) {
      ribbon->_creationDataStorage->idx = vertexData->_idx;
    }
    ribbon->_creationDataStorage->closePath  = closePath;
    ribbon->_creationDataStorage->closeArray = closeArray;

    vertexData->applyToMesh(*ribbon, updatable);

    return ribbon;
  }
}

MeshPtr MeshBuilder::CreateCylinder(const std::string& name,
                                    CylinderOptions& options, Scene* scene)
{
  auto cylinder = Mesh::New(name, scene);

  options.sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  cylinder->_originalBuilderSideOrientation = options.sideOrientation;

  auto vertexData = VertexData::CreateCylinder(options);

  vertexData->applyToMesh(*cylinder, options.updatable);

  return cylinder;
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
  const auto arc          = options.arc();
  const auto& closed      = options.closed;
  const auto& shape       = options.shape;
  const auto& radius      = options.radius;
  const auto tessellation = static_cast<float>(options.tessellation);
  const auto clip         = static_cast<float>(options.clip);
  const auto& updatable   = options.updatable;

  const unsigned int sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  const auto& cap = options.cap;
  const auto& pi2 = Math::PI2;
  std::vector<std::vector<Vector3>> paths;
  const auto& invertUV = options.invertUV;

  const float step = pi2 / tessellation * arc;
  Vector3 rotated;
  for (float i = 0.f; i <= tessellation - clip; ++i) {
    std::vector<Vector3> path;
    if (cap == Mesh::CAP_START || cap == Mesh::CAP_ALL) {
      path.emplace_back(Vector3(0.f, shape[0].y, 0.f));
      path.emplace_back(Vector3(std::cos(i * step) * shape[0].x * radius,
                                shape[0].y,
                                std::sin(i * step) * shape[0].x * radius));
    }
    for (std::size_t p = 0; p < shape.size(); ++p) {
      rotated = Vector3(std::cos(i * step) * shape[p].x * radius, shape[p].y,
                        std::sin(i * step) * shape[p].x * radius);
      path.emplace_back(rotated);
    }
    if (cap == Mesh::CAP_END || cap == Mesh::CAP_ALL) {
      path.emplace_back(
        Vector3(std::cos(i * step) * shape[shape.size() - 1].x * radius,
                shape[shape.size() - 1].y,
                std::sin(i * step) * shape[shape.size() - 1].x * radius));
      path.emplace_back(Vector3(0.f, shape[shape.size() - 1].y, 0.f));
    }
    paths.emplace_back(path);
  }

  // lathe ribbon
  RibbonOptions ribbonOptions(paths);
  ribbonOptions.closeArray      = closed;
  ribbonOptions.sideOrientation = sideOrientation;
  ribbonOptions.updatable       = updatable;
  ribbonOptions.invertUV        = invertUV;
  auto lathe = MeshBuilder::CreateRibbon(name, ribbonOptions, scene);
  return lathe;
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
        BABYLON_LOG_ERROR("Tools", msg);
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
  const auto& path = options.path;
  auto radius      = 1.f;
  if (options.radius.has_value()) {
    radius = *options.radius;
  }
  else if (options.instance) {
    radius = options.instance->_creationDataStorage->radius;
  }
  const auto& tessellation
    = options.tessellation == 0 ? 64 : options.tessellation;
  const auto& radiusFunction = options.radiusFunction;
  auto cap                   = options.cap;
  const auto& invertUV       = options.invertUV;
  const auto& updatable      = options.updatable;
  const auto sideOrientation
    = MeshBuilder::updateSideOrientation(options.sideOrientation);
  auto instance  = options.instance;
  const auto arc = options.arc();

  // tube geometry
  const auto tubePathArray
    = [](const std::vector<Vector3>& _path, Path3D& path3D,
         std::vector<std::vector<Vector3>>& circlePaths, float _radius,
         unsigned int _tessellation,
         const std::function<float(unsigned int i, float distance)>&
           _radiusFunction,
         unsigned int _cap, float _arc) {
        auto& tangents        = path3D.getTangents();
        const auto& normals   = path3D.getNormals();
        const auto& distances = path3D.getDistances();
        const auto& pi2       = Math::PI2;
        const auto step       = pi2 / static_cast<float>(_tessellation) * _arc;

        auto rad = 0.f;
        Vector3 normal;
        Vector3 rotated;
        Matrix rotationMatrix = Tmp::MatrixArray[0];
        // TODO FIXME
        unsigned int index
          = (_cap == Mesh::NO_CAP || _cap == Mesh::CAP_END) ? 0 : 0;
        circlePaths.resize(_path.size() + index);
        for (unsigned int i = 0; i < _path.size(); ++i) {
          rad = (_radiusFunction == nullptr) ?
                  _radius :
                  _radiusFunction(i, distances[i]); // current radius
          std::vector<Vector3> circlePath;          // current circle array
          normal = normals[i];                      // current normal
          for (std::size_t t = 0; t < _tessellation; ++t) {
            Matrix::RotationAxisToRef(tangents[i], step * static_cast<float>(t),
                                      rotationMatrix);
            rotated
              = (t + 1 <= circlePath.size()) ? circlePath[t] : Vector3::Zero();
            Vector3::TransformCoordinatesToRef(normal, rotationMatrix, rotated);
            rotated.scaleInPlace(rad).addInPlace(_path[i]);
            circlePath.emplace_back(rotated);
          }
          circlePaths[index] = circlePath;
          ++index;
        }
        // cap
        const auto capPath
          = [_path](unsigned int nbPoints, unsigned int pathIndex) {
              std::vector<Vector3> pointCap;
              for (std::size_t i = 0; i < nbPoints; ++i) {
                pointCap.emplace_back(_path[pathIndex]);
              }
              return pointCap;
            };
        switch (_cap) {
          case Mesh::NO_CAP:
            break;
          case Mesh::CAP_START:
            circlePaths[0] = capPath(_tessellation, 0);
            circlePaths[1] = circlePaths[2];
            break;
          case Mesh::CAP_END:
            circlePaths[index]     = circlePaths[index - 1];
            circlePaths[index + 1] = capPath(
              _tessellation, static_cast<unsigned int>(_path.size() - 1));
            break;
          case Mesh::CAP_ALL:
            circlePaths[0]         = capPath(_tessellation, 0);
            circlePaths[1]         = circlePaths[2];
            circlePaths[index]     = circlePaths[index - 1];
            circlePaths[index + 1] = capPath(
              _tessellation, static_cast<unsigned int>(_path.size() - 1));
            break;
          default:
            break;
        }
        return circlePaths;
      };
  Path3D path3D;
  std::vector<std::vector<Vector3>> pathArray;
  if (instance) {
    // tube update
    auto& storage = instance->_creationDataStorage;
    auto iArc     = (options.arc() != 0.f) ? options.arc() : storage->arc;
    path3D        = storage->path3D.update(path);
    pathArray     = tubePathArray(path, path3D, storage->pathArray, radius,
                              storage->tessellation, radiusFunction,
                              storage->cap, iArc);
    RibbonOptions ribbonOptions(pathArray);
    instance = MeshBuilder::CreateRibbon("", ribbonOptions, scene);
    // Update mode, no need to recreate the storage.
    storage->path3D    = path3D;
    storage->pathArray = pathArray;
    storage->arc       = iArc;
    storage->radius    = radius;

    return instance;
  }
  // tube creation
  path3D = Path3D(path);
  std::vector<std::vector<Vector3>> newPathArray;
  cap       = (cap > 3) ? 0 : cap;
  pathArray = tubePathArray(path, path3D, newPathArray, radius, tessellation,
                            radiusFunction, cap, arc);
  RibbonOptions ribbonOptions(pathArray);
  ribbonOptions.closePath       = true;
  ribbonOptions.closeArray      = false;
  ribbonOptions.updatable       = updatable;
  ribbonOptions.sideOrientation = sideOrientation;
  ribbonOptions.invertUV        = invertUV;
  auto tube = MeshBuilder::CreateRibbon(name, ribbonOptions, scene);
  tube->_creationDataStorage->pathArray    = pathArray;
  tube->_creationDataStorage->path3D       = path3D;
  tube->_creationDataStorage->tessellation = tessellation;
  tube->_creationDataStorage->cap          = cap;
  tube->_creationDataStorage->arc          = options.arc();
  tube->_creationDataStorage->radius       = radius;

  return tube;
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
  const auto indices = sourceMesh->getIndices();
  const auto positions
    = sourceMesh->getVerticesData(VertexBuffer::PositionKind);
  auto normals         = sourceMesh->getVerticesData(VertexBuffer::NormalKind);
  const auto& position = options.position;
  auto normal          = options.normal;
  const auto& size     = options.size;
  const auto& angle    = options.angle;

  // Getting correct rotation
  if (options.calculateNormal) {
    Vector3 target(0.f, 0.f, 1.f);
    auto camera = sourceMesh->getScene()->activeCamera;
    auto cameraWorldTarget
      = Vector3::TransformCoordinates(target, camera->getWorldMatrix());

    normal = camera->globalPosition().subtract(cameraWorldTarget);
  }

  auto yaw   = -std::atan2(normal.z, normal.x) - Math::PI_2;
  auto len   = std::sqrt(normal.x * normal.x + normal.z * normal.z);
  auto pitch = std::atan2(normal.y, len);

  // Matrix
  auto decalWorldMatrix
    = Matrix::RotationYawPitchRoll(yaw, pitch, angle)
        .multiply(Matrix::Translation(position.x, position.y, position.z));
  auto inverseDecalWorldMatrix = Matrix::Invert(decalWorldMatrix);
  auto meshWorldMatrix         = sourceMesh->getWorldMatrix();
  auto transformMatrix = meshWorldMatrix.multiply(inverseDecalWorldMatrix);

  auto vertexData = std::make_unique<VertexData>();
  vertexData->indices.clear();
  vertexData->positions.clear();
  vertexData->normals.clear();
  vertexData->uvs.clear();

  unsigned int currentVertexDataIndex = 0;

  const auto extractDecalVector3 = [&](std::size_t indexId) {
    PositionNormalVertex result;
    if (indices.empty() || positions.empty() || normals.empty()) {
      return result;
    }
    const auto& vertexId = indices[indexId];
    result.position
      = Vector3(positions[vertexId * 3], positions[vertexId * 3 + 1],
                positions[vertexId * 3 + 2]);
    // Send vector to decal local world
    result.position
      = Vector3::TransformCoordinates(result.position, transformMatrix);
    // Get normal
    result.normal
      = Vector3(normals[vertexId * 3 + 0], normals[vertexId * 3 + 1],
                normals[vertexId * 3 + 2]);
    result.normal = Vector3::TransformNormal(result.normal, transformMatrix);
    return result;
  };

  // Inspired by
  // https://github.com/mrdoob/three.js/blob/eee231960882f6f3b6113405f524956145148146/examples/js/geometries/DecalGeometry.js
  const auto clip = [&size](const std::vector<PositionNormalVertex>& vertices,
                            const Vector3& axis) {
    if (vertices.empty()) {
      return vertices;
    }

    auto clipSize = 0.5f * std::abs(Vector3::Dot(size, axis));

    const auto clipVertices
      = [&](const PositionNormalVertex& v0, const PositionNormalVertex& v1) {
          auto clipFactor
            = Vector3::GetClipFactor(v0.position, v1.position, axis, clipSize);

          return PositionNormalVertex(
            Vector3::Lerp(v0.position, v1.position, clipFactor),
            Vector3::Lerp(v0.normal, v1.normal, clipFactor));
        };

    std::vector<PositionNormalVertex> result;

    bool v1Out, v2Out, v3Out;
    unsigned int total;
    std::optional<PositionNormalVertex> nV1, nV2, nV3, nV4;
    float d1, d2, d3;
    for (std::size_t index = 0; index < vertices.size(); index += 3) {
      total = 0;

      nV1 = std::nullopt;
      nV2 = std::nullopt;
      nV3 = std::nullopt;
      nV4 = std::nullopt;

      d1 = Vector3::Dot(vertices[index].position, axis) - clipSize;
      d2 = Vector3::Dot(vertices[index + 1].position, axis) - clipSize;
      d3 = Vector3::Dot(vertices[index + 2].position, axis) - clipSize;

      v1Out = d1 > 0;
      v2Out = d2 > 0;
      v3Out = d3 > 0;

      total = (v1Out ? 1 : 0) + (v2Out ? 1 : 0) + (v3Out ? 1 : 0);

      switch (total) {
        case 0:
          result.emplace_back(vertices[index + 0]);
          result.emplace_back(vertices[index + 1]);
          result.emplace_back(vertices[index + 2]);
          break;
        case 1:

          if (v1Out) {
            nV1 = vertices[index + 1];
            nV2 = vertices[index + 2];
            nV3 = clipVertices(vertices[index], *nV1);
            nV4 = clipVertices(vertices[index], *nV2);
          }

          if (v2Out) {
            nV1 = vertices[index + 0];
            nV2 = vertices[index + 2];
            nV3 = clipVertices(vertices[index + 1], *nV1);
            nV4 = clipVertices(vertices[index + 1], *nV2);

            result.emplace_back(*nV3);
            result.emplace_back(*nV2);
            result.emplace_back(*nV1);

            result.emplace_back(*nV2);
            result.emplace_back(*nV3);
            result.emplace_back(*nV4);
            break;
          }
          if (v3Out) {
            nV1 = vertices[index + 0];
            nV2 = vertices[index + 1];
            nV3 = clipVertices(vertices[index + 2], *nV1);
            nV4 = clipVertices(vertices[index + 2], *nV2);
          }

          if (nV1 && nV2 && nV3 && nV4) {
            result.emplace_back(*nV1);
            result.emplace_back(*nV2);
            result.emplace_back(*nV3);

            result.emplace_back(*nV4);
            result.emplace_back(*nV3);
            result.emplace_back(*nV2);
          }
          break;
        case 2:
          if (!v1Out) {
            nV1 = vertices[index + 0];
            nV2 = clipVertices(*nV1, vertices[index + 1]);
            nV3 = clipVertices(*nV1, vertices[index + 2]);
            result.emplace_back(*nV1);
            result.emplace_back(*nV2);
            result.emplace_back(*nV3);
          }
          if (!v2Out) {
            nV1 = vertices[index + 1];
            nV2 = clipVertices(*nV1, vertices[index + 2]);
            nV3 = clipVertices(*nV1, vertices[index + 0]);
            result.emplace_back(*nV1);
            result.emplace_back(*nV2);
            result.emplace_back(*nV3);
          }
          if (!v3Out) {
            nV1 = vertices[index + 2];
            nV2 = clipVertices(*nV1, vertices[index + 0]);
            nV3 = clipVertices(*nV1, vertices[index + 1]);
            result.emplace_back(*nV1);
            result.emplace_back(*nV2);
            result.emplace_back(*nV3);
          }
          break;
        case 3:
          break;
        default:
          break;
      }
    }

    return result;
  };

  for (std::size_t index = 0; index < indices.size(); index += 3) {
    std::vector<PositionNormalVertex> faceVertices;

    faceVertices.emplace_back(extractDecalVector3(index));
    faceVertices.emplace_back(extractDecalVector3(index + 1));
    faceVertices.emplace_back(extractDecalVector3(index + 2));

    // Clip
    faceVertices = clip(faceVertices, Vector3(1.f, 0.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(-1.f, 0.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(0.f, 1.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(0.f, -1.f, 0.f));
    faceVertices = clip(faceVertices, Vector3(0.f, 0.f, 1.f));
    faceVertices = clip(faceVertices, Vector3(0.f, 0.f, -1.f));

    if (faceVertices.empty()) {
      continue;
    }

    // Add UVs and get back to world
    for (std::size_t vIndex = 0; vIndex < faceVertices.size(); ++vIndex) {
      auto& vertex = faceVertices[vIndex];

      vertexData->indices.emplace_back(currentVertexDataIndex);
      vertex.position.toArray(vertexData->positions,
                              currentVertexDataIndex * 3);
      vertex.normal.toArray(vertexData->normals, currentVertexDataIndex * 3);
      vertexData->uvs.emplace_back(0.5f + vertex.position.x / size.x);
      vertexData->uvs.emplace_back(0.5f + vertex.position.y / size.y);

      ++currentVertexDataIndex;
    }
  }

  // Return mesh
  auto decal = Mesh::New(name, sourceMesh->getScene());
  vertexData->applyToMesh(*decal);

  // decal->setPosition(position.clone());
  decal->rotation = Vector3(pitch, yaw, angle);

  return decal;
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
  RibbonOptions ribbonOptions(pathArray);
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
