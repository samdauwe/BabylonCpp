#include <babylon/materials/material_defines.h>

namespace BABYLON {

MaterialDefines::MaterialDefines()
    : NUM_BONE_INFLUENCERS{0}, BonesPerMesh{0}, LIGHTMAPEXCLUDED{false}
{
}

MaterialDefines::~MaterialDefines()
{
}

bool MaterialDefines::operator[](unsigned int define) const
{
  return (define < defines.size()) ? defines[define] : false;
}

void MaterialDefines::resizeLights(unsigned int lightIndex)
{
  if (lightIndex >= lights.size()) {
    for (size_t i = lights.size(); i <= lightIndex; ++i) {
      lights.emplace_back(false);
      pointlights.emplace_back(false);
      dirlights.emplace_back(false);
      hemilights.emplace_back(false);
      spotlights.emplace_back(false);
      shadows.emplace_back(false);
      shadowesms.emplace_back(false);
      shadowpcfs.emplace_back(false);
      lightmapexcluded.emplace_back(false);
      lightmapnospecular.emplace_back(false);
    }
  }
}

std::ostream& operator<<(std::ostream& os,
                         const MaterialDefines& materialDefines)
{
  for (size_t i = 0; i < materialDefines.defines.size(); ++i) {
    if (materialDefines.defines[i]) {
      os << "#define " << materialDefines._keys[i] << "\n";
    }
  }

  os << "#define NUM_BONE_INFLUENCERS " << materialDefines.NUM_BONE_INFLUENCERS
     << "\n";
  os << "#define BonesPerMesh " << materialDefines.BonesPerMesh << "\n";

  for (size_t i = 0; i < materialDefines.lights.size(); ++i) {
    if (materialDefines.lights[i]) {
      os << "#define LIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.pointlights.size(); ++i) {
    if (materialDefines.pointlights[i]) {
      os << "#define POINTLIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.dirlights.size(); ++i) {
    if (materialDefines.dirlights[i]) {
      os << "#define DIRLIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.hemilights.size(); ++i) {
    if (materialDefines.hemilights[i]) {
      os << "#define HEMILIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.spotlights.size(); ++i) {
    if (materialDefines.spotlights[i]) {
      os << "#define SPOTLIGHT" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadows.size(); ++i) {
    if (materialDefines.shadows[i]) {
      os << "#define SHADOW" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowesms.size(); ++i) {
    if (materialDefines.shadowesms[i]) {
      os << "#define SHADOWESM" << i << "\n";
    }
  }

  for (size_t i = 0; i < materialDefines.shadowpcfs.size(); ++i) {
    if (materialDefines.shadowpcfs[i]) {
      os << "#define SHADOWPCF" << i << "\n";
    }
  }

  return os;
}

bool MaterialDefines::isEqual(MaterialDefines& other) const
{
  if ((defines.size() != other.defines.size())
      || (_keys.size() != other._keys.size())
      || (lights.size() != other.lights.size())
      || (pointlights.size() != other.pointlights.size())
      || (dirlights.size() != other.dirlights.size())
      || (hemilights.size() != other.hemilights.size())
      || (spotlights.size() != other.spotlights.size())
      || (shadows.size() != other.shadows.size())
      || (shadowesms.size() != other.shadowesms.size())
      || (shadowpcfs.size() != other.shadowpcfs.size())) {
    return false;
  }

  if ((NUM_BONE_INFLUENCERS != other.NUM_BONE_INFLUENCERS)
      || (BonesPerMesh != other.BonesPerMesh)) {
    return false;
  }

  for (size_t i = 0; i < defines.size(); ++i) {
    if (defines[i] != other.defines[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < _keys.size(); ++i) {
    if (_keys[i] != other._keys[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < lights.size(); ++i) {
    if (lights[i] != other.lights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < pointlights.size(); ++i) {
    if (pointlights[i] != other.pointlights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < dirlights.size(); ++i) {
    if (dirlights[i] != other.dirlights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < hemilights.size(); ++i) {
    if (hemilights[i] != other.hemilights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < spotlights.size(); ++i) {
    if (spotlights[i] != other.spotlights[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadows.size(); ++i) {
    if (shadows[i] != other.shadows[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowesms.size(); ++i) {
    if (shadowesms[i] != other.shadowesms[i]) {
      return false;
    }
  }

  for (size_t i = 0; i < shadowpcfs.size(); ++i) {
    if (shadowpcfs[i] != other.shadowpcfs[i]) {
      return false;
    }
  }

  return true;
}

void MaterialDefines::cloneTo(MaterialDefines& other)
{
  other.defines = defines;
  other._keys   = _keys;

  other.NUM_BONE_INFLUENCERS = NUM_BONE_INFLUENCERS;
  other.BonesPerMesh         = BonesPerMesh;

  other.lights      = lights;
  other.pointlights = pointlights;
  other.dirlights   = dirlights;
  other.hemilights  = hemilights;
  other.spotlights  = spotlights;
  other.shadows     = shadows;
  other.shadowesms  = shadowesms;
  other.shadowpcfs  = shadowpcfs;
}

void MaterialDefines::reset()
{
  for (size_t i = 0; i < defines.size(); ++i) {
    defines[i] = false;
  }

  NUM_BONE_INFLUENCERS = 0;
  BonesPerMesh         = 0;

  lights.clear();
  pointlights.clear();
  dirlights.clear();
  hemilights.clear();
  spotlights.clear();
  shadows.clear();
  shadowesms.clear();
  shadowpcfs.clear();
}

std::string MaterialDefines::toString() const
{
  std::ostringstream oss;
  oss << *this;

  return oss.str();
}

} // end of namespace BABYLON
