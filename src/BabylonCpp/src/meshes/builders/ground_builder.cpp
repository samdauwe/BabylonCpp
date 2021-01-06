#include <babylon/meshes/builders/ground_builder.h>

#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/ground_mesh.h>
#include <babylon/meshes/vertex_data.h>
#include <babylon/misc/file_tools.h>

namespace BABYLON {

GroundMeshPtr GroundBuilder::CreateGround(const std::string& name, GroundOptions& options,
                                          Scene* scene)
{
  const auto ground = GroundMesh::New(name, scene);
  ground->_setReady(false);
  ground->_subdivisionsX = options.subdivisionsX.value_or(options.subdivisions.value_or(1));
  ground->_subdivisionsY = options.subdivisionsY.value_or(options.subdivisions.value_or(1));
  ground->_width         = static_cast<float>(options.width.value_or(1.f));
  ground->_height        = static_cast<float>(options.height.value_or(1.f));
  ground->_maxX          = ground->_width / 2.f;
  ground->_maxZ          = ground->_height / 2.f;
  ground->_minX          = -ground->_maxX;
  ground->_minZ          = -ground->_maxZ;

  const auto vertexData = VertexData::CreateGround(options);

  vertexData->applyToMesh(*ground, options.updatable);

  ground->_setReady(true);

  return ground;
}

MeshPtr GroundBuilder::CreateTiledGround(const std::string& name, TiledGroundOptions& options,
                                         Scene* scene)
{
  const auto tiledGround = Mesh::New(name, scene);

  const auto vertexData = VertexData::CreateTiledGround(options);

  vertexData->applyToMesh(*tiledGround, options.updatable);

  return tiledGround;
}

GroundMeshPtr GroundBuilder::CreateGroundFromHeightMap(const std::string& name,
                                                       const std::string& url,
                                                       GroundFromHeightMapOptions& options,
                                                       Scene* scene)
{
  const auto width        = options.width.value_or(10.f);
  const auto height       = options.height.value_or(10.f);
  const auto subdivisions = options.subdivisions.value_or(1);
  const auto minHeight    = options.minHeight.value_or(0.f);
  const auto maxHeight    = options.maxHeight.value_or(1.f);
  const auto filter       = options.colorFilter.value_or(Color3(0.3f, 0.59f, 0.11f));
  const auto alphaFilter  = options.alphaFilter.value_or(0.f);
  const auto& updatable   = options.updatable;
  const auto& onReady     = options.onReady;

  const auto ground      = GroundMesh::New(name, scene);
  ground->_subdivisionsX = subdivisions;
  ground->_subdivisionsY = subdivisions;
  ground->_width         = width;
  ground->_height        = height;
  ground->_maxX          = ground->_width / 2.f;
  ground->_maxZ          = ground->_height / 2.f;
  ground->_minX          = -ground->_maxX;
  ground->_minZ          = -ground->_maxZ;

  ground->_setReady(false);

  const auto onload = [scene, width, height, subdivisions, minHeight, maxHeight, filter,
                       alphaFilter, ground, updatable, onReady](const Image& img) {
    if (scene->isDisposed()) {
      return;
    }

    // Create VertexData from map data
    GroundFromHeightMapOptions options;
    options.width        = width;
    options.height       = height;
    options.subdivisions = subdivisions;
    options.minHeight    = minHeight;
    options.maxHeight    = maxHeight;
    options.colorFilter  = filter;
    options.buffer       = img.data;
    options.bufferWidth  = static_cast<unsigned int>(img.width);
    options.bufferHeight = static_cast<unsigned int>(img.height);
    options.alphaFilter  = alphaFilter;

    auto vertexData = VertexData::CreateGroundFromHeightMap(options);

    vertexData->applyToMesh(*ground, updatable);

    // execute ready callback, if set
    if (onReady) {
      onReady(ground);
    }

    ground->_setReady(true);
  };

  const auto onError = [](const std::string& msg, const std::string& /*exception*/) {
    BABYLON_LOG_ERROR("Tools", msg)
  };

  FileTools::LoadImageFromUrl(url, onload, onError, false);

  return ground;
}

} // end of namespace BABYLON
