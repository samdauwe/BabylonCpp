#ifndef BABYLON_MESH_LINE_MESH_H
#define BABYLON_MESH_LINE_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT LinesMesh : public Mesh {

public:
  template <typename... Ts>
  static LinesMesh* New(Ts&&... args)
  {
    auto mesh = new LinesMesh(std::forward<Ts>(args)...);
    mesh->addToScene(static_cast<unique_ptr_t<AbstractMesh>>(mesh));

    return mesh;
  }
  ~LinesMesh() override;

  /**
   * @brief Returns the string "LineMesh".
   */
  const string_t getClassName() const override;

  IReflect::Type type() const override;

  InstancedMesh* createInstance(const string_t& name);
  void _bind(SubMesh* subMesh, Effect* effect, unsigned int fillMode) override;
  void _draw(SubMesh* subMesh, int fillMode, size_t instancesCount = 0,
             bool alternate = false) override;
  PickingInfo intersects(const Ray& ray, bool fastCheck = true) override;
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

  /**
   * @brief Returns a new LineMesh object cloned from the current one.
   */
  LinesMesh* clone(const string_t& name, Node* newParent = nullptr,
                   bool doNotCloneChildren = false);

protected:
  LinesMesh(const string_t& name, Scene* scene, Node* parent = nullptr,
            LinesMesh* source = nullptr, bool doNotCloneChildren = true,
            bool useVertexColor = false, bool useVertexAlpha = false);

  /**
   * Hidden
   */
  Material*& get_material() override;

  /**
   * Hidden
   */
  void set_material(Material* const& material) override;

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
  Material* _colorShaderMaterial;
  ShaderMaterial* _colorShader;

}; // end of class LinesMesh

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_LINE_MESH_H
