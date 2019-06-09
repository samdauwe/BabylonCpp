#ifndef BABYLON_MESHES_TRAIL_MESH_H
#define BABYLON_MESHES_TRAIL_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

class TrailMesh;
using TrailMeshPtr = std::shared_ptr<TrailMesh>;

/**
 * @brief Class used to create a trail following a mesh.
 */
class BABYLON_SHARED_EXPORT TrailMesh : public Mesh {

public:
  template <typename... Ts>
  static TrailMeshPtr New(Ts&&... args)
  {
    auto mesh
      = std::shared_ptr<TrailMesh>(new TrailMesh(std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~TrailMesh() override;

  /**
   * @brief "TrailMesh"
   * @returns "TrailMesh"
   */
  const std::string getClassName() const override;

  /**
   * @brief Start trailing mesh.
   */
  void start();

  /**
   * @brief Stop trailing mesh.
   */
  void stop();

  /**
   * @brief Update trailing mesh geometry.
   */
  void update();

  /**
   * @brief Returns a new TrailMesh object.
   * @param name is a string, the name given to the new mesh
   * @param newGenerator use new generator object for cloned trail mesh
   * @returns a new mesh
   */
  TrailMeshPtr clone(const std::string& name             = "",
                     const AbstractMeshPtr& newGenerator = nullptr);

  /**
   * @brief Serializes this trail mesh.
   * @param serializationObject object to write serialization to
   */
  void serialize(json& serializationObject) const override;

  /**
   * @brief Parses a serialized trail mesh.
   * @param parsedMesh the serialized mesh
   * @param scene the scene to create the trail mesh in
   * @returns the created trail mesh
   */
  static TrailMeshPtr Parse(const json& parsedMesh, Scene* scene);

protected:
  /**
   * @brief Constructor
   * @param name The value used by scene.getMeshByName() to do a lookup.
   * @param generator The mesh to generate a trail.
   * @param scene The scene to add this mesh to.
   * @param diameter Diameter of trailing mesh. Default is 1.
   * @param length Length of trailing mesh. Default is 60.
   * @param autoStart Automatically start trailing mesh. Default true.
   */
  TrailMesh(const std::string& name, const AbstractMeshPtr& generator,
            Scene* scene, float diameter = 1.f, float length = 60.f,
            bool autoStart = true);

private:
  void _createMesh();

private:
  AbstractMeshPtr _generator;
  bool _autoStart;
  bool _running;
  float _diameter;
  float _length;
  uint32_t _sectionPolygonPointsCount;
  std::vector<Vector3> _sectionVectors;
  std::vector<Vector3> _sectionNormalVectors;
  Observer<Scene>::Ptr _beforeRenderObserver;

}; // end of class TrailMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_TRAIL_MESH_H
