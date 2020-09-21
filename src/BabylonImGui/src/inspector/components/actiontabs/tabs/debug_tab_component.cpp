#include <babylon/inspector/components/actiontabs/tabs/debug_tab_component.h>

#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/line_container_component.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

DebugTabComponent::DebugTabComponent(const IPaneComponentProps& iProps)
    : PaneComponent{iProps}, _physicsViewersEnabled{false}
{
  componentWillMount();
}

DebugTabComponent::~DebugTabComponent()
{
  componentWillUnmount();
}

void DebugTabComponent::componentWillMount()
{
}

void DebugTabComponent::componentWillUnmount()
{
}

void DebugTabComponent::switchPhysicsViewers()
{
}

void DebugTabComponent::render()
{
  const auto scene = props.scene;

  if (!scene) {
    return;
  }

  // --- HELPERS ---
  static auto helpersOpenedClosed = false;
  if (LineContainerComponent::render("HELPERS", helpersOpenedClosed)) {
    if (CheckBoxLineComponent::render("Physics", _physicsViewersEnabled)) {
      switchPhysicsViewers();
    }
  }
  // --- TEXTURE CHANNELS ---
  static auto textureChannelsClosed = false;
  ImGui::SetNextTreeNodeOpen(textureChannelsClosed, ImGuiCond_Always);
  if (LineContainerComponent::render("CORE TEXTURE CHANNELS", textureChannelsClosed)) {
    if (CheckBoxLineComponent::render("Diffuse", StandardMaterial::DiffuseTextureEnabled())) {
      StandardMaterial::SetDiffuseTextureEnabled(!StandardMaterial::DiffuseTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Ambient", StandardMaterial::AmbientTextureEnabled())) {
      StandardMaterial::SetAmbientTextureEnabled(!StandardMaterial::AmbientTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Specular", StandardMaterial::SpecularTextureEnabled())) {
      StandardMaterial::SetSpecularTextureEnabled(!StandardMaterial::SpecularTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Emissive", StandardMaterial::EmissiveTextureEnabled())) {
      StandardMaterial::SetEmissiveTextureEnabled(!StandardMaterial::EmissiveTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Bump", StandardMaterial::BumpTextureEnabled())) {
      StandardMaterial::SetBumpTextureEnabled(!StandardMaterial::BumpTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Opacity", StandardMaterial::OpacityTextureEnabled())) {
      StandardMaterial::SetOpacityTextureEnabled(!StandardMaterial::OpacityTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Reflection", StandardMaterial::ReflectionTextureEnabled())) {
      StandardMaterial::SetReflectionTextureEnabled(!StandardMaterial::ReflectionTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Refraction", StandardMaterial::RefractionTextureEnabled())) {
      StandardMaterial::SetRefractionTextureEnabled(!StandardMaterial::RefractionTextureEnabled());
    }
    if (CheckBoxLineComponent::render("ColorGrading",
                                      StandardMaterial::ColorGradingTextureEnabled())) {
      StandardMaterial::SetColorGradingTextureEnabled(
        !StandardMaterial::ColorGradingTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Lightmap", StandardMaterial::LightmapTextureEnabled())) {
      StandardMaterial::SetLightmapTextureEnabled(!StandardMaterial::LightmapTextureEnabled());
    }
    if (CheckBoxLineComponent::render("Fresnel", StandardMaterial::FresnelEnabled())) {
      StandardMaterial::SetFresnelEnabled(!StandardMaterial::FresnelEnabled());
    }
  }
  // --- FEATURES ---
  static auto featuresClosed = false;
  if (LineContainerComponent::render("FEATURES", featuresClosed)) {
    if (CheckBoxLineComponent::render("Animations", scene->animationsEnabled)) {
      scene->animationsEnabled = !scene->animationsEnabled;
    }
    if (CheckBoxLineComponent::render("Physics", scene->physicsEnabled)) {
      scene->physicsEnabled = !scene->physicsEnabled;
    }
    if (CheckBoxLineComponent::render("Collisions", scene->collisionsEnabled)) {
      scene->collisionsEnabled = !scene->collisionsEnabled;
    }
    if (CheckBoxLineComponent::render("Fog", scene->fogEnabled)) {
      scene->fogEnabled = !scene->fogEnabled;
    }
    if (CheckBoxLineComponent::render("Lens flares", scene->lensFlaresEnabled)) {
      scene->lensFlaresEnabled = !scene->lensFlaresEnabled;
    }
    if (CheckBoxLineComponent::render("Lights", scene->lightsEnabled)) {
      scene->lightsEnabled = !scene->lightsEnabled;
    }
    if (CheckBoxLineComponent::render("Particles", scene->particlesEnabled)) {
      scene->particlesEnabled = !scene->particlesEnabled;
    }
    if (CheckBoxLineComponent::render("Post-processes", scene->postProcessesEnabled)) {
      scene->postProcessesEnabled = !scene->postProcessesEnabled;
    }
    if (CheckBoxLineComponent::render("Probes", scene->probesEnabled)) {
      scene->probesEnabled = !scene->probesEnabled;
    }
    if (CheckBoxLineComponent::render("Textures", scene->texturesEnabled)) {
      scene->texturesEnabled = !scene->texturesEnabled;
    }
    if (CheckBoxLineComponent::render("Procedural textures", scene->proceduralTexturesEnabled)) {
      scene->proceduralTexturesEnabled = !scene->proceduralTexturesEnabled;
    }
    if (CheckBoxLineComponent::render("Render targets", scene->renderTargetsEnabled)) {
      scene->renderTargetsEnabled = !scene->renderTargetsEnabled;
    }
    if (CheckBoxLineComponent::render("Shadows", scene->shadowsEnabled)) {
      scene->shadowsEnabled = !scene->shadowsEnabled;
    }
    if (CheckBoxLineComponent::render("Skeletons", scene->skeletonsEnabled)) {
      scene->skeletonsEnabled = !scene->skeletonsEnabled;
    }
    if (CheckBoxLineComponent::render("Sprites", scene->spritesEnabled)) {
      scene->spritesEnabled = !scene->spritesEnabled;
    }
  }
}

} // end of namespace BABYLON
