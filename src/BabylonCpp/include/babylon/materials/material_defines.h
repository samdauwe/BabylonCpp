#ifndef BABYLON_MATERIALS_MATERIAL_DEFINES_H
#define BABYLON_MATERIALS_MATERIAL_DEFINES_H

#include <babylon/babylon_global.h>
#include <babylon/materials/imaterial_defines.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT MaterialDefines : public IMaterialDefines {

  MaterialDefines();
  virtual ~MaterialDefines();

  bool operator[](unsigned int define) const;
  void resizeLights(unsigned int lightIndex);
  friend std::ostream& operator<<(std::ostream& os,
                                  const MaterialDefines& materialDefines);

  virtual bool isEqual(MaterialDefines& other) const override;
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
  std::vector<bool> shadowvsms;
  std::vector<bool> shadowpcfs;

  bool LIGHTMAPEXCLUDED;
  std::vector<bool> lightmapexcluded;
  std::vector<bool> lightmapnospecular;

}; // end of struct MaterialDefines

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_MATERIAL_DEFINES_H
