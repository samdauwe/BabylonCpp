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
  void markAsImageProcessingDirty() override;
  void markAsLightDirty() override;
  void markAsAttributesDirty() override;
  void markAsTexturesDirty() override;
  void markAsFresnelDirty() override;
  void markAsMiscDirty() override;
  virtual void rebuild() override;
  virtual bool isEqual(const MaterialDefines& other) const override;
  virtual void cloneTo(MaterialDefines& other) override;
  virtual void reset() override;
  virtual string_t toString() const override;

  // Properties
  vector_t<bool> defines;
  vector_t<string_t> _keys;

  unsigned int NUM_BONE_INFLUENCERS;
  unsigned int BonesPerMesh;
  unsigned int NUM_MORPH_INFLUENCERS;

  unsigned int DIFFUSEDIRECTUV;
  unsigned int AMBIENTDIRECTUV;
  unsigned int OPACITYDIRECTUV;
  unsigned int EMISSIVEDIRECTUV;
  unsigned int SPECULARDIRECTUV;
  unsigned int BUMPDIRECTUV;
  unsigned int LIGHTMAPDIRECTUV;

  // PBR Material
  unsigned int ALBEDODIRECTUV;
  unsigned int REFLECTIVITYDIRECTUV;
  unsigned int MICROSURFACEMAPDIRECTUV;
  float ALPHATESTVALUE;

  vector_t<bool> lights;
  vector_t<bool> pointlights;
  vector_t<bool> dirlights;
  vector_t<bool> hemilights;
  vector_t<bool> spotlights;
  vector_t<bool> shadows;
  vector_t<bool> shadowesms;
  vector_t<bool> shadowcloseesms;
  vector_t<bool> shadowpcfs;
  vector_t<bool> shadowcubes;

  bool TANGENT;
  bool SHADOWS;
  bool LIGHTMAPEXCLUDED;
  vector_t<bool> lightmapexcluded;
  vector_t<bool> lightmapnospecular;

  bool USERIGHTHANDEDSYSTEM;

  bool _isDirty;
  int _renderId;

  bool _areLightsDirty;
  bool _areAttributesDirty;
  bool _areTexturesDirty;
  bool _areFresnelDirty;
  bool _areMiscDirty;
  bool _areImageProcessingDirty;

  bool _normals;
  bool _uvs;

  bool _needNormals;
  bool _needUVs;

}; // end of struct MaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_DEFINES_H
