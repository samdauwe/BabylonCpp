#include <babylon/materials/imaterial_detail_map_defines.h>

namespace BABYLON {

IMaterialDetailMapDefines::IMaterialDetailMapDefines()
{
  boolDef = {
    {"DETAIL", false}, //
  };

  intDef = {
    {"DETAILDIRECTUV", 0},           //
    {"DETAIL_NORMALBLENDMETHOD", 0}, //
  };
}

IMaterialDetailMapDefines::~IMaterialDetailMapDefines() = default;

} // end of namespace BABYLON
