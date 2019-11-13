#include <babylon/materialslibrary/water/water_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

WaterMaterialDefines::WaterMaterialDefines() : MaterialDefines{}
{
  boolDef = {
    {"BUMP", false},                  //
    {"REFLECTION", false},            //
    {"CLIPPLANE", false},             //
    {"CLIPPLANE2", false},            //
    {"CLIPPLANE3", false},            //
    {"CLIPPLANE4", false},            //
    {"ALPHATEST", false},             //
    {"DEPTHPREPASS", false},          //
    {"POINTSIZE", false},             //
    {"FOG", false},                   //
    {"NORMAL", false},                //
    {"UV1", false},                   //
    {"UV2", false},                   //
    {"VERTEXCOLOR", false},           //
    {"VERTEXALPHA", false},           //
    {"INSTANCES", false},             //
    {"SPECULARTERM", false},          //
    {"LOGARITHMICDEPTH", false},      //
    {"FRESNELSEPARATE", false},       //
    {"BUMPSUPERIMPOSE", false},       //
    {"BUMPAFFECTSREFLECTION", false}, //
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

WaterMaterialDefines::~WaterMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
