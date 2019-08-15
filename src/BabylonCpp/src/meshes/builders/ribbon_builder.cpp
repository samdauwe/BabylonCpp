#include <babylon/meshes/builders/ribbon_builder.h>

#include <babylon/math/tmp.h>
#include <babylon/meshes/_creation_data_storage.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_buffer.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr RibbonBuilder::CreateRibbon(const std::string& name,
                                    RibbonOptions& options, Scene* scene)
{
  const auto& pathArray  = options.pathArray;
  const auto& closeArray = options.closeArray;
  const auto& closePath  = options.closePath;
  auto sideOrientation
    = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  const auto& instance  = options.instance;
  const auto& updatable = options.updatable;

  if (instance) { // existing ribbon instance update
                  // positionFunction : ribbon case
    // only pathArray and sideOrientation parameters are taken into account for
    // positions update
    auto& minimum
      = Tmp::Vector3Array[0].setAll(std::numeric_limits<float>::max());
    auto& maximum
      = Tmp::Vector3Array[1].setAll(std::numeric_limits<float>::lowest());
    const auto positionFunction = [&](Float32Array& positions) -> void {
      auto minlg = pathArray[0].size();
      auto& mesh = instance;
      auto i     = 0u;
      auto ns
        = (mesh->_originalBuilderSideOrientation == Mesh::DOUBLESIDE) ? 2u : 1u;
      for (auto si = 1ull; si <= ns; ++si) {
        for (auto p = 0ull; p < pathArray.size(); ++p) {
          const auto& path = pathArray[p];
          const auto l     = path.size();
          minlg            = (minlg < l) ? minlg : l;
          for (auto j = 0u; j < minlg; ++j) {
            const auto& pathPoint = path[j];
            positions[i]          = pathPoint.x;
            positions[i + 1]      = pathPoint.y;
            positions[i + 2]      = pathPoint.z;
            minimum.minimizeInPlaceFromFloats(pathPoint.x, pathPoint.y,
                                              pathPoint.z);
            maximum.maximizeInPlaceFromFloats(pathPoint.x, pathPoint.y,
                                              pathPoint.z);
            i += 3;
          }
          if (mesh->_creationDataStorage
              && mesh->_creationDataStorage->closePath) {
            const auto& pathPoint = path[0];
            positions[i]          = pathPoint.x;
            positions[i + 1]      = pathPoint.y;
            positions[i + 2]      = pathPoint.z;
            i += 3;
          }
        }
      }
    };
    auto positions = instance->getVerticesData(VertexBuffer::PositionKind);
    positionFunction(positions);
    if (instance->_boundingInfo) {
      instance->_boundingInfo->reConstruct(minimum, maximum,
                                           instance->_worldMatrix);
    }
    else {
      instance->_boundingInfo = std::make_shared<BoundingInfo>(
        minimum, maximum, instance->_worldMatrix);
    }
    instance->updateVerticesData(VertexBuffer::PositionKind, positions, false,
                                 false);
    if (!options.colors.empty()) {
      auto colors = instance->getVerticesData(VertexBuffer::ColorKind);
      for (auto c = 0ull, colorIndex = 0ull; c < options.colors.size();
           c++, colorIndex += 4) {
        const auto& color      = options.colors[c];
        colors[colorIndex]     = color.r;
        colors[colorIndex + 1] = color.g;
        colors[colorIndex + 2] = color.b;
        colors[colorIndex + 3] = color.a;
      }
      instance->updateVerticesData(VertexBuffer::ColorKind, colors, false,
                                   false);
    }
    if (!options.uvs.empty()) {
      auto uvs = instance->getVerticesData(VertexBuffer::UVKind);
      for (size_t i = 0ull; i < options.uvs.size(); ++i) {
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
        auto indexFirst = 0u;
        auto indexLast  = 0ull;
        for (auto p = 0ull; p < pathArray.size(); ++p) {
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

} // end of namespace BABYLON
