#include <babylon/materialslibrary/lava/lava_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

LavaMaterialDefines::LavaMaterialDefines()
{
  boolDef = {
    {"DIFFUSE", false},                   //
    {"CLIPPLANE", false},                 //
    {"CLIPPLANE2", false},                //
    {"CLIPPLANE3", false},                //
    {"CLIPPLANE4", false},                //
    {"CLIPPLANE5", false},                //
    {"CLIPPLANE6", false},                //
    {"ALPHATEST", false},                 //
    {"DEPTHPREPASS", false},              //
    {"POINTSIZE", false},                 //
    {"FOG", false},                       //
    {"LIGHT0", false},                    //
    {"LIGHT1", false},                    //
    {"LIGHT2", false},                    //
    {"LIGHT3", false},                    //
    {"SPOTLIGHT0", false},                //
    {"SPOTLIGHT1", false},                //
    {"SPOTLIGHT2", false},                //
    {"SPOTLIGHT3", false},                //
    {"HEMILIGHT0", false},                //
    {"HEMILIGHT1", false},                //
    {"HEMILIGHT2", false},                //
    {"HEMILIGHT3", false},                //
    {"DIRLIGHT0", false},                 //
    {"DIRLIGHT1", false},                 //
    {"DIRLIGHT2", false},                 //
    {"DIRLIGHT3", false},                 //
    {"POINTLIGHT0", false},               //
    {"POINTLIGHT1", false},               //
    {"POINTLIGHT2", false},               //
    {"POINTLIGHT3", false},               //
    {"SHADOW0", false},                   //
    {"SHADOW1", false},                   //
    {"SHADOW2", false},                   //
    {"SHADOW3", false},                   //
    {"SHADOWS", false},                   //
    {"SHADOWESM0", false},                //
    {"SHADOWESM1", false},                //
    {"SHADOWESM2", false},                //
    {"SHADOWESM3", false},                //
    {"SHADOWPOISSON0", false},            //
    {"SHADOWPOISSON1", false},            //
    {"SHADOWPOISSON2", false},            //
    {"SHADOWPOISSON3", false},            //
    {"SHADOWPCF0", false},                //
    {"SHADOWPCF1", false},                //
    {"SHADOWPCF2", false},                //
    {"SHADOWPCF3", false},                //
    {"SHADOWPCSS0", false},               //
    {"SHADOWPCSS1", false},               //
    {"SHADOWPCSS2", false},               //
    {"SHADOWPCSS3", false},               //
    {"NORMAL", false},                    //
    {"UV1", false},                       //
    {"UV2", false},                       //
    {"VERTEXCOLOR", false},               //
    {"VERTEXALPHA", false},               //
    {"INSTANCES", false},                 //
    {"UNLIT", false},                     //
    {"IMAGEPROCESSINGPOSTPROCESS", false} //
  };

  intDef = {
    {"NUM_BONE_INFLUENCERS", 0}, //
    {"BonesPerMesh", 0}          //
  };
}

LavaMaterialDefines::~LavaMaterialDefines() = default;

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
