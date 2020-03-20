#include <babylon/meshes/builders/plane_builder.h>

#include <babylon/maths/plane.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh.h>
#include <babylon/meshes/vertex_data.h>

namespace BABYLON {

MeshPtr PlaneBuilder::CreatePlane(const std::string& name, PlaneOptions& options, Scene* scene)
{
  auto plane = Mesh::New(name, scene);

  options.sideOrientation = Mesh::_GetDefaultSideOrientation(options.sideOrientation);
  plane->_originalBuilderSideOrientation = *options.sideOrientation;

  auto vertexData = VertexData::CreatePlane(options);

  vertexData->applyToMesh(*plane, options.updatable);

  if (options.sourcePlane) {
    plane->translate(options.sourcePlane->normal, -options.sourcePlane->d);
    plane->setDirection(options.sourcePlane->normal.scale(-1.f));
  }

  return plane;
}

} // end of namespace BABYLON
