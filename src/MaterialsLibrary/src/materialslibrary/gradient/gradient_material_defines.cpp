#include <babylon/materialslibrary/gradient/gradient_material_defines.h>

namespace BABYLON {
namespace MaterialsLibrary {

GradientMaterialDefines::GradientMaterialDefines() : MaterialDefines{}
{
  _keys = {"DIFFUSE",
           "CLIPPLANE",
           "ALPHATEST",
           "POINTSIZE",
           "FOG",
           "LIGHT0",
           "LIGHT1",
           "LIGHT2",
           "LIGHT3",
           "SPOTLIGHT0",
           "SPOTLIGHT1",
           "SPOTLIGHT2",
           "SPOTLIGHT3",
           "HEMILIGHT0",
           "HEMILIGHT1",
           "HEMILIGHT2",
           "HEMILIGHT3",
           "DIRLIGHT0",
           "DIRLIGHT1",
           "DIRLIGHT2",
           "DIRLIGHT3",
           "POINTLIGHT0",
           "POINTLIGHT1",
           "POINTLIGHT2",
           "POINTLIGHT3",
           "SHADOW0",
           "SHADOW1",
           "SHADOW2",
           "SHADOW3",
           "SHADOWS",
           "SHADOWESM0",
           "SHADOWESM1",
           "SHADOWESM2",
           "SHADOWESM3",
           "SHADOWPCF0",
           "SHADOWPCF1",
           "SHADOWPCF2",
           "SHADOWPCF3",
           "NORMAL",
           "UV1",
           "UV2",
           "VERTEXCOLOR",
           "VERTEXALPHA",
           "INSTANCES",
           "LOGARITHMICDEPTH",
           "SPECULARTERM",
           "SHADOWFULLFLOAT"};
  rebuild();
}

GradientMaterialDefines::~GradientMaterialDefines()
{
}

} // end of namespace MaterialsLibrary
} // end of namespace BABYLON
