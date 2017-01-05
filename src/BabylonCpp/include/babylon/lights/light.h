#ifndef BABYLON_LIGHTS_LIGHT_H
#define BABYLON_LIGHTS_LIGHT_H

#include <babylon/babylon_global.h>
#include <babylon/engine/node.h>
#include <babylon/math/color3.h>
#include <babylon/math/matrix.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Light : public Node {

public:
  // LightmapMode Consts
  /**
   * If every light affecting the material is in this lightmapMode,
   * material.lightmapTexture adds or multiplies
   * (depends on material.useLightmapAsShadowmap)
   * after every other light calculations.
   */
  static constexpr unsigned int LIGHTMAP_DEFAULT = 0;
  /**
   * If every light affecting the material is in this lightmapMode,
   * material.lightmapTexture adds or multiplies
   * (depends on material.useLightmapAsShadowmap)
   * after every other light calculations.
   */
  static constexpr unsigned int LIGHTMAP_SPECULAR = 1;
  /**
   * material.lightmapTexture as only diffuse lighting from this light
   * adds pnly specular lighting from this light
   * adds dynamic shadows
   */
  static constexpr unsigned int LIGHTMAP_SHADOWSONLY = 2;

public:
  ~Light();

  virtual IReflect::Type type() const override;
  void addToScene(std::unique_ptr<Light>&& newLight);

  /**
   * @param {boolean} fullDetails - support for multiple levels of logging
   * within scene loading
   */
  std::string toString(bool fullDetails = false) const;
  virtual ShadowGenerator* getShadowGenerator();
  virtual Vector3 getAbsolutePosition();
  virtual void transferToEffect(Effect* effect,
                                const std::string& uniformName0);
  virtual void transferToEffect(Effect* effect, const std::string& uniformName0,
                                const std::string& uniformName1);
  virtual Matrix* _getWorldMatrix();
  bool canAffectMesh(AbstractMesh* mesh);
  Matrix* getWorldMatrix() override;
  virtual void dispose(bool doNotRecurse = false) override;
  virtual unsigned int getTypeID() const;
  std::unique_ptr<Light> clone(const std::string& name);
  Json::object serialize() const;

  // Statics
  static Light* Parse(const Json::value& parsedLight, Scene* scene);

protected:
  Light(const std::string& name, Scene* scene);

public:
  Color3 diffuse;
  Color3 specular;
  float intensity;
  float range;
  unsigned int includeOnlyWithLayerMask;
  std::vector<AbstractMesh*> includedOnlyMeshes;
  std::vector<AbstractMesh*> excludedMeshes;
  unsigned int excludeWithLayerMask;
  unsigned int lightmapMode;
  float radius;
  ShadowGenerator* _shadowGenerator;
  std::vector<std::string> _excludedMeshesIds;
  std::vector<std::string> _includedOnlyMeshesIds;

private:
  std::unique_ptr<Matrix> _parentedWorldMatrix;
  std::unique_ptr<Matrix> _worldMatrix;

}; // end of class Light

} // end of namespace BABYLON

#endif // end of BABYLON_LIGHTS_LIGHT_H
