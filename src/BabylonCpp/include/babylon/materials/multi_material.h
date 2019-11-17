#ifndef BABYLON_MATERIALS_MULTI_MATERIAL_H
#define BABYLON_MATERIALS_MULTI_MATERIAL_H

#include <babylon/babylon_api.h>
#include <babylon/materials/material.h>

namespace BABYLON {

class MultiMaterial;
using MultiMaterialPtr = std::shared_ptr<MultiMaterial>;

/**
 * @brief A multi-material is used to apply different materials to different
 * parts of the same object without the need of separate meshes. This can be use
 * to improve performances.
 * @see http://doc.babylonjs.com/how_to/multi_materials
 */
class BABYLON_SHARED_EXPORT MultiMaterial : public Material {

public:
  template <typename... Ts>
  static MultiMaterialPtr New(Ts&&... args)
  {
    auto material = std::shared_ptr<MultiMaterial>(new MultiMaterial(std::forward<Ts>(args)...));
    material->addMultiMaterialToScene(material);

    return material;
  }
  ~MultiMaterial() override; // = default

  /**
   * @brief Function used to align with Node.getChildren().
   * @returns the list of Materials used within the multi material
   */
  std::vector<MaterialPtr>& getChildren();

  /**
   * @brief Hidden
   */
  Type type() const override;

  /**
   * @brief Get one of the submaterial by its index in the submaterials array.
   * @param index The index to look the sub material at
   * @returns The Material if the index has been defined
   */
  MaterialPtr& getSubMaterial(unsigned int index);

  /**
   * @brief Get the list of active textures for the whole sub materials list.
   * @returns All the textures that will be used during the rendering
   */
  std::vector<BaseTexturePtr> getActiveTextures() const override;

  /**
   * @brief Gets the current class name of the material e.g. "MultiMaterial"
   * Mainly use in serialization.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Checks if the material is ready to render the requested sub mesh.
   * @param mesh Define the mesh the submesh belongs to
   * @param subMesh Define the sub mesh to look readyness for
   * @param useInstances Define whether or not the material is used with
   * instances
   * @returns true if ready, otherwise false
   */
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;

  /**
   * @brief Clones the current material and its related sub materials.
   * @param name Define the name of the newly cloned material
   * @param cloneChildren Define if submaterial will be cloned or shared with
   * the parent instance
   * @returns the cloned material
   */
  MaterialPtr clone(const std::string& _name, bool cloneChildren = false) const override;

  /**
   * @brief Serializes the materials into a JSON representation.
   * @returns the JSON representation
   */
  json serialize() const;

  /**
   * @brief Dispose the material and release its associated resources.
   * @param forceDisposeEffect Define if we want to force disposing the
   * associated effect (if false the shader is not released and could be reuse
   * later on)
   * @param forceDisposeTextures Define if we want to force disposing the
   * associated textures (if false, they will not be disposed and can still be
   * use elsewhere in the app)
   * @param forceDisposeChildren Define if we want to force disposing the
   * associated submaterials (if false, they will not be disposed and can still
   * be use elsewhere in the app)
   */
  void dispose(bool forceDisposeEffect = false, bool forceDisposeTextures = false,
               bool forceDisposeChildren = false) override;

  /**
   * @brief Creates a MultiMaterial from parsed MultiMaterial data.
   * @param parsedMultiMaterial defines parsed MultiMaterial data.
   * @param scene defines the hosting scene
   * @returns a new MultiMaterial
   */
  static MultiMaterialPtr ParseMultiMaterial(const json& parsedMultiMaterial, Scene* scene);

protected:
  /**
   * @brief Instantiates a new Multi Material.
   * A multi-material is used to apply different materials to different parts of
   * the same object without the need of separate meshes. This can be use to
   * improve performances.
   * @see http://doc.babylonjs.com/how_to/multi_materials
   * @param name Define the name in the scene
   * @param scene Define the scene the material belongs to
   */
  MultiMaterial(const std::string name, Scene* scene);

  /**
   * @brief Gets the list of Materials used within the multi material.
   * They need to be ordered according to the submeshes order in the associated
   * mesh
   */
  std::vector<MaterialPtr>& get_subMaterials();

  /**
   * @brief Sets the list of Materials used within the multi material.
   * They need to be ordered according to the submeshes order in the associated
   * mesh
   */
  void set_subMaterials(const std::vector<MaterialPtr>& value);

private:
  void _hookArray(const std::vector<MaterialPtr>& array);

public:
  /**
   * Gets or Sets the list of Materials used within the multi material.
   * They need to be ordered according to the submeshes order in the associated
   * mesh
   */
  Property<MultiMaterial, std::vector<MaterialPtr>> subMaterials;

private:
  std::vector<MaterialPtr> _subMaterials;

}; // end of class MultiMaterial

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MULTI_MATERIAL_H
