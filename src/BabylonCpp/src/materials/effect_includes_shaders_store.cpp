#include <babylon/materials/effect_includes_shaders_store.h>

#include <babylon/shaders/shadersinclude/instancesVertex_fx.h>
#include <babylon/shaders/shadersinclude/fogFragment_fx.h>
#include <babylon/shaders/shadersinclude/clipPlaneVertexDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/logDepthDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/logDepthVertex_fx.h>
#include <babylon/shaders/shadersinclude/colorCurves_fx.h>
#include <babylon/shaders/shadersinclude/bumpFragmentFunctions_fx.h>
#include <babylon/shaders/shadersinclude/bonesVertex_fx.h>
#include <babylon/shaders/shadersinclude/logDepthFragment_fx.h>
#include <babylon/shaders/shadersinclude/colorGrading_fx.h>
#include <babylon/shaders/shadersinclude/fogVertex_fx.h>
#include <babylon/shaders/shadersinclude/bonesDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/pbrLightFunctions_fx.h>
#include <babylon/shaders/shadersinclude/clipPlaneVertex_fx.h>
#include <babylon/shaders/shadersinclude/shadowsVertex_fx.h>
#include <babylon/shaders/shadersinclude/pbrFunctions_fx.h>
#include <babylon/shaders/shadersinclude/colorGradingDefinition_fx.h>
#include <babylon/shaders/shadersinclude/colorCurvesDefinition_fx.h>
#include <babylon/shaders/shadersinclude/helperFunctions_fx.h>
#include <babylon/shaders/shadersinclude/reflectionFunction_fx.h>
#include <babylon/shaders/shadersinclude/fresnelFunction_fx.h>
#include <babylon/shaders/shadersinclude/pointCloudVertex_fx.h>
#include <babylon/shaders/shadersinclude/bumpFragment_fx.h>
#include <babylon/shaders/shadersinclude/harmonicsFunctions_fx.h>
#include <babylon/shaders/shadersinclude/pbrShadowFunctions_fx.h>
#include <babylon/shaders/shadersinclude/pointCloudVertexDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/clipPlaneFragmentDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/lightsFragmentFunctions_fx.h>
#include <babylon/shaders/shadersinclude/clipPlaneFragment_fx.h>
#include <babylon/shaders/shadersinclude/pbrLightFunctionsCall_fx.h>
#include <babylon/shaders/shadersinclude/lightFragmentDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/fogFragmentDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/instancesDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/lightFragment_fx.h>
#include <babylon/shaders/shadersinclude/shadowsVertexDeclaration_fx.h>
#include <babylon/shaders/shadersinclude/shadowsFragmentFunctions_fx.h>
#include <babylon/shaders/shadersinclude/fogVertexDeclaration_fx.h>

namespace BABYLON {

std::unordered_map<std::string, const char*> EffectIncludesShadersStore::Shaders  
 = {{"instancesVertex", instancesVertex},
   {"fogFragment", fogFragment},
   {"clipPlaneVertexDeclaration", clipPlaneVertexDeclaration},
   {"logDepthDeclaration", logDepthDeclaration},
   {"logDepthVertex", logDepthVertex},
   {"colorCurves", colorCurves},
   {"bumpFragmentFunctions", bumpFragmentFunctions},
   {"bonesVertex", bonesVertex},
   {"logDepthFragment", logDepthFragment},
   {"colorGrading", colorGrading},
   {"fogVertex", fogVertex},
   {"bonesDeclaration", bonesDeclaration},
   {"pbrLightFunctions", pbrLightFunctions},
   {"clipPlaneVertex", clipPlaneVertex},
   {"shadowsVertex", shadowsVertex},
   {"pbrFunctions", pbrFunctions},
   {"colorGradingDefinition", colorGradingDefinition},
   {"colorCurvesDefinition", colorCurvesDefinition},
   {"helperFunctions", helperFunctions},
   {"reflectionFunction", reflectionFunction},
   {"fresnelFunction", fresnelFunction},
   {"pointCloudVertex", pointCloudVertex},
   {"bumpFragment", bumpFragment},
   {"harmonicsFunctions", harmonicsFunctions},
   {"pbrShadowFunctions", pbrShadowFunctions},
   {"pointCloudVertexDeclaration", pointCloudVertexDeclaration},
   {"clipPlaneFragmentDeclaration", clipPlaneFragmentDeclaration},
   {"lightsFragmentFunctions", lightsFragmentFunctions},
   {"clipPlaneFragment", clipPlaneFragment},
   {"pbrLightFunctionsCall", pbrLightFunctionsCall},
   {"lightFragmentDeclaration", lightFragmentDeclaration},
   {"fogFragmentDeclaration", fogFragmentDeclaration},
   {"instancesDeclaration", instancesDeclaration},
   {"lightFragment", lightFragment},
   {"shadowsVertexDeclaration", shadowsVertexDeclaration},
   {"shadowsFragmentFunctions", shadowsFragmentFunctions},
   {"fogVertexDeclaration", fogVertexDeclaration}
};

} // end of namespace BABYLON
