#ifndef BABYLON_MESH_LINE_MESH_H
#define BABYLON_MESH_LINE_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/math/color3.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

class ShaderMaterial;
using ShaderMaterialPtr = std::shared_ptr<ShaderMaterial>;

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT LinesMesh : public Mesh {

public:
  template <typename... Ts>
  static LinesMeshPtr New(Ts&&... args)
  {
    auto mesh
      = std::shared_ptr<LinesMesh>(new LinesMesh(::std::forward<Ts>(args)...));
    mesh->addToScene(mesh);

    return mesh;
  }
  ~LinesMesh() override;

  /**
   * @brief Returns the string "LineMesh".
   */
  const std::string getClassName() const override;

  IReflect::Type type() const override;

  InstancedMesh* createInstance(const std::string& name);

  /**
   * @brief Hidden
   */
  void _bind(SubMesh* subMesh, Effect* effect, unsigned int fillMode) override;

  /**
   * @brief Hidden
   */
  void _draw(SubMesh* subMesh, int fillMode, size_t instancesCount = 0,
             bool alternate = false) override;

  PickingInfo intersects(const Ray& ray, bool fastCheck = true) override;
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Returns a new LineMesh object cloned from the current one.
   */
  LinesMeshPtr clone(const std::string& name, Node* newParent = nullptr,
                     bool doNotCloneChildren = false);

  /**
   * @brief Enables the edge rendering mode on the mesh.
   * This mode makes the mesh edges visible
   * @param epsilon defines the maximal distance between two angles to detect a
   * face
   * @param checkVerticesInsteadOfIndices indicates that we should check vertex
   * list directly instead of faces
   * @returns the currentAbstractMesh
   * @see https://www.babylonjs-playground.com/#19O9TU#0
   */
  AbstractMesh& enableEdgesRendering(float epsilon = 0.95f,
                                     bool checkVerticesInsteadOfIndices
                                     = false);

protected:
  LinesMesh(const std::string& name, Scene* scene, Node* parent = nullptr,
            LinesMesh* source = nullptr, bool doNotCloneChildren = true,
            bool useVertexColor = false, bool useVertexAlpha = false);

  /**
   * Hidden
   */
  MaterialPtr& get_material() override;

  /**
   * Hidden
   */
  void set_material(const MaterialPtr& material) override;

  /**
   * Hidden
   */
  bool get_checkCollisions() const override;

  /**
   * @brief The intersection Threshold is the margin applied when intersection a
   * segment of the LinesMesh with a Ray.
   * This margin is expressed in world space coordinates, so its value may vary.
   * Default value is 0.1
   * @returns the intersection Threshold value.
   */
  float get_intersectionThreshold() const;

  /**
   * @brief The intersection Threshold is the margin applied when intersection a
   * segment of the LinesMesh with a Ray.
   * This margin is expressed in world space coordinates, so its value may vary.
   * @param value the new threshold to apply
   */
  void set_intersectionThreshold(float value);

public:
  float dashSize;
  float gapSize;
  Color3 color;
  float alpha;
  bool useVertexColor;
  bool useVertexAlpha;

  /**
   * The margin applied when intersection a segment of the LinesMesh with a Ray
   */
  Property<LinesMesh, float> intersectionThreshold;

private:
  float _intersectionThreshold;
  MaterialPtr _colorShaderMaterial;
  ShaderMaterialPtr _colorShader;

}; // end of class LinesMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_LINE_MESH_H
