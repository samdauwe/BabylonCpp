#include <babylon/inspector/components/actiontabs/tabs/debug_tab_component.h>

#include <babylon/engine/scene.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

DebugTabComponent::DebugTabComponent(const IPaneComponentProps& iProps)
    : PaneComponent{iProps}
    , _skeletonViewersEnabled{false}
    , _physicsViewersEnabled{false}
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

void DebugTabComponent::switchSkeletonViewers()
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
  static auto helpersOpened = true;
  ImGui::SetNextTreeNodeOpen(helpersOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("HELPERS")) {
    CheckBoxLineComponent::render(
      "Bones", _skeletonViewersEnabled,
      [this](bool /*value*/) { switchSkeletonViewers(); });
    CheckBoxLineComponent::render(
      "Physics", _physicsViewersEnabled,
      [this](bool /*value*/) { switchPhysicsViewers(); });
    helpersOpened = true;
  }
  else {
    helpersOpened = false;
  }
  // --- TEXTURE CHANNELS ---
  static auto textureChannelsOpened = true;
  ImGui::SetNextTreeNodeOpen(textureChannelsOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("TEXTURE CHANNELS")) {
    CheckBoxLineComponent::render(
      "Diffuse", StandardMaterial::DiffuseTextureEnabled(), [](bool /*value*/) {
        StandardMaterial::SetDiffuseTextureEnabled(
          !StandardMaterial::DiffuseTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Ambient", StandardMaterial::AmbientTextureEnabled(), [](bool /*value*/) {
        StandardMaterial::SetAmbientTextureEnabled(
          !StandardMaterial::AmbientTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Specular", StandardMaterial::SpecularTextureEnabled(),
      [](bool /*value*/) {
        StandardMaterial::SetSpecularTextureEnabled(
          !StandardMaterial::SpecularTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Emissive", StandardMaterial::EmissiveTextureEnabled(),
      [](bool /*value*/) {
        StandardMaterial::SetEmissiveTextureEnabled(
          !StandardMaterial::EmissiveTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Bump", StandardMaterial::BumpTextureEnabled(), [](bool /*value*/) {
        StandardMaterial::SetBumpTextureEnabled(
          !StandardMaterial::BumpTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Opacity", StandardMaterial::OpacityTextureEnabled(), [](bool /*value*/) {
        StandardMaterial::SetOpacityTextureEnabled(
          !StandardMaterial::OpacityTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Reflection", StandardMaterial::ReflectionTextureEnabled(),
      [](bool /*value*/) {
        StandardMaterial::SetReflectionTextureEnabled(
          !StandardMaterial::ReflectionTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Refraction", StandardMaterial::RefractionTextureEnabled(),
      [](bool /*value*/) {
        StandardMaterial::SetRefractionTextureEnabled(
          !StandardMaterial::RefractionTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "ColorGrading", StandardMaterial::ColorGradingTextureEnabled(),
      [](bool /*value*/) {
        StandardMaterial::SetColorGradingTextureEnabled(
          !StandardMaterial::ColorGradingTextureEnabled());
      });
    CheckBoxLineComponent::render(
      "Lightmap", StandardMaterial::LightmapTextureEnabled(),
      [](bool /*value*/) {
        StandardMaterial::SetLightmapTextureEnabled(
          !StandardMaterial::LightmapTextureEnabled());
      });
    CheckBoxLineComponent::render("Fresnel", StandardMaterial::FresnelEnabled(),
                                  [](bool /*value*/) {
                                    StandardMaterial::SetFresnelEnabled(
                                      !StandardMaterial::FresnelEnabled());
                                  });
    textureChannelsOpened = true;
  }
  else {
    textureChannelsOpened = false;
  }
  // --- FEATURES ---
  static auto featuresOpened = true;
  ImGui::SetNextTreeNodeOpen(featuresOpened, ImGuiCond_Always);
  if (ImGui::CollapsingHeader("FEATURES")) {
    CheckBoxLineComponent::render(
      "Animations", scene->animationsEnabled, [&scene](bool /*value*/) {
        scene->animationsEnabled = !scene->animationsEnabled;
      });
    CheckBoxLineComponent::render(
      "Collisions", scene->collisionsEnabled, [&scene](bool /*value*/) {
        scene->collisionsEnabled = !scene->collisionsEnabled;
      });
    CheckBoxLineComponent::render(
      "Fog", scene->fogEnabled,
      [&scene](bool /*value*/) { scene->fogEnabled = !scene->fogEnabled; });
    CheckBoxLineComponent::render(
      "Lens flares", scene->lensFlaresEnabled, [&scene](bool /*value*/) {
        scene->lensFlaresEnabled = !scene->lensFlaresEnabled;
      });
    CheckBoxLineComponent::render(
      "Lights", scene->lightsEnabled, [&scene](bool /*value*/) {
        scene->lightsEnabled = !scene->lightsEnabled;
      });
    CheckBoxLineComponent::render(
      "Particles", scene->particlesEnabled, [&scene](bool /*value*/) {
        scene->particlesEnabled = !scene->particlesEnabled;
      });
    CheckBoxLineComponent::render(
      "Post-processes", scene->postProcessesEnabled, [&scene](bool /*value*/) {
        scene->postProcessesEnabled = !scene->postProcessesEnabled;
      });
    CheckBoxLineComponent::render(
      "Probes", scene->probesEnabled, [&scene](bool /*value*/) {
        scene->probesEnabled = !scene->probesEnabled;
      });
    CheckBoxLineComponent::render(
      "Textures", scene->texturesEnabled, [&scene](bool /*value*/) {
        scene->texturesEnabled = !scene->texturesEnabled;
      });
    CheckBoxLineComponent::render(
      "Procedural textures", scene->proceduralTexturesEnabled,
      [&scene](bool /*value*/) {
        scene->proceduralTexturesEnabled = !scene->proceduralTexturesEnabled;
      });
    CheckBoxLineComponent::render(
      "Render targets", scene->renderTargetsEnabled, [&scene](bool /*value*/) {
        scene->renderTargetsEnabled = !scene->renderTargetsEnabled;
      });
    CheckBoxLineComponent::render(
      "Shadows", scene->shadowsEnabled, [&scene](bool /*value*/) {
        scene->shadowsEnabled = !scene->shadowsEnabled;
      });
    CheckBoxLineComponent::render(
      "Skeletons", scene->skeletonsEnabled, [&scene](bool /*value*/) {
        scene->skeletonsEnabled = !scene->skeletonsEnabled;
      });
    CheckBoxLineComponent::render(
      "Sprites", scene->spritesEnabled, [&scene](bool /*value*/) {
        scene->spritesEnabled = !scene->spritesEnabled;
      });
    featuresOpened = true;
  }
  else {
    featuresOpened = false;
  }
}

} // end of namespace BABYLON
