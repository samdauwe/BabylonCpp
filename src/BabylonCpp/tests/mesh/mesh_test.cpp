#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../test_utils.h"

#include <babylon/meshes/mesh.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/mesh_builder.h>


TEST(Mesh, clone)
{
  using namespace BABYLON;
  {
    std::unique_ptr<BABYLON::Engine> engine = createSubject();
    auto scene = BABYLON::Scene::New(engine.get());
    SphereOptions sphereOptions;
    sphereOptions.diameter = 0.2f;
    auto sphereLight = MeshBuilder::CreateSphere("sphere", sphereOptions, scene.get());


    // This commented line will cause a segmentation fault (bad weak ptr)
    // if uncommented!
    //auto ss = sphereLight->clone("");

    /*
    will fail in
    SubMeshPtr Mesh::_createGlobalSubMesh(bool force)
      return SubMesh::New(0, 0, totalVertices, 0, getTotalIndices(),
                    shared_from_base<Mesh>() <== fail here!
                    );
    */

    //auto ss2 = SubMesh::New(0, 0, 0, 0, 0, sphereLight.shared_from_this<Mesh>());

  }

}
