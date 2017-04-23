#ifndef BABYLON_MATERIALS_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/imaterial_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT MaterialDefines : public IMaterialDefines {

  MaterialDefines();
  virtual ~MaterialDefines();

  bool operator[](unsigned int define) const;
  bool operator==(const MaterialDefines& rhs) const;
  bool operator!=(const MaterialDefines& rhs) const;
  void resizeLights(unsigned int lightIndex);
  friend std::ostream& operator<<(std::ostream& os,
                                  const MaterialDefines& materialDefines);

  bool isDirty() const override;
  void markAsProcessed() override;
  void markAsUnprocessed() override;
  void markAllAsDirty() override;
  void markAsLightDirty() override;
  void markAsAttributesDirty() override;
  void markAsTexturesDirty() override;
  void markAsFresnelDirty() override;
  void markAsMiscDirty() override;
  void rebuild() override;
  virtual bool isEqual(const MaterialDefines& other) const override;
  virtual void cloneTo(MaterialDefines& other) override;
  virtual void reset() override;
  virtual std::string toString() const override;

  // Properties
  std::vector<bool> defines;
  std::vector<std::string> _keys;

  std::size_t NUM_BONE_INFLUENCERS;
  std::size_t BonesPerMesh;

  std::vector<bool> lights;
  std::vector<bool> pointlights;
  std::vector<bool> dirlights;
  std::vector<bool> hemilights;
  std::vector<bool> spotlights;
  std::vector<bool> shadows;
  std::vector<bool> shadowesms;
  std::vector<bool> shadowpcfs;

  bool LIGHTMAPEXCLUDED;
  std::vector<bool> lightmapexcluded;
  std::vector<bool> lightmapnospecular;

  bool _isDirty;
  int _renderId;

  bool _areLightsDirty;
  bool _areAttributesDirty;
  bool _areTexturesDirty;
  bool _areFresnelDirty;
  bool _areMiscDirty;

  bool _normals;
  bool _uvs;

  bool _needNormals;
  bool _needUVs;

}; // end of struct MaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_DEFINES_H
