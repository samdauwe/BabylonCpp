#ifndef BABYLON_MATERIALS_LIBRARY_GRADIENT_GRADIENT_MATERIAL_H
#define BABYLON_MATERIALS_LIBRARY_GRADIENT_GRADIENT_MATERIAL_H

#include <babylon/babylon_global.h>
#include <babylon/materials/push_material.h>
#include <babylon/materialslibrary/gradient/gradient_material_defines.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace MaterialsLibrary {

class BABYLON_SHARED_EXPORT GradientMaterial : public PushMaterial {

public:
  /**
   * constructor
   * @param name The name given to the material in order to identify it
   * afterwards.
   * @param scene The scene the material is used in.
   */
  GradientMaterial(const std::string& name, Scene* scene);
  ~GradientMaterial() override;

  /**
   * Returns wehter or not the grid requires alpha blending.
   */
  bool needAlphaBlending() const override;
  bool needAlphaTesting() const override;
  BaseTexturePtr getAlphaTestTexture() override;
  bool isReadyForSubMesh(AbstractMesh* mesh, BaseSubMesh* subMesh,
                         bool useInstances = false) override;
  void bindForSubMesh(Matrix* world, Mesh* mesh, SubMesh* subMesh) override;
  std::vector<IAnimatablePtr> getAnimatables();
  const string_t getClassName() const override;
  virtual void dispose(bool forceDisposeEffect   = false,
                       bool forceDisposeTextures = false) override;
  MaterialPtr clone(const std::string& name,
                    bool cloneChildren = false) const override;
  Json::object serialize() const;

  /** Statics **/
  static GradientMaterial* Parse(const Json::value& source, Scene* scene,
                                 const std::string& rootUrl);

protected:
  unsigned int get_maxSimultaneousLights() const;
  void set_maxSimultaneousLights(unsigned int value);

public:
  Property<GradientMaterial, unsigned int> maxSimultaneousLights;
  // The gradient top color, red by default
  Color3 topColor;
  float topColorAlpha;
  Color3 bottomColor;
  float bottomColorAlpha;
  float offset;
  float scale;
  float smoothness;
  bool disableLighting;

private:
  unsigned int _maxSimultaneousLights;
  Color3 _scaledDiffuse;
  int _renderId;

}; // end of class GradientMaterial

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_GRADIENT_GRADIENT_MATERIAL_H
