#include <babylon/inspector/tabs/scene_tab.h>

#include <babylon/engine/scene.h>
#include <babylon/imgui/imgui_dock.h>
#include <babylon/inspector/inspector.h>
#include <babylon/materials/standard_material.h>

namespace BABYLON {

SceneTab::SceneTab(Inspector& inspector) : Tab{"Scene"}, _inspector{inspector}
{
  _scene = inspector.scene();
}

SceneTab::~SceneTab()
{
}

void SceneTab::render()
{
  if (!_scene) {
    return;
  }

  if (ImGui::BeginDock("Scene")) {
    // General
    if (ImGui::CollapsingHeader("General", "General", true, true)) {
      ImGui::Checkbox("Bounding boxes", &_scene->forceShowBoundingBoxes);
    }
    // Rendering Mode
    if (ImGui::CollapsingHeader("Rendering Mode", "Rendering Mode", true,
                                true)) {
      // Determine current render mode
      int currenRenderMode = -1;
      if (!_scene->forceWireframe && !_scene->forcePointsCloud()) {
        // Solid render mode
        currenRenderMode = 0;
      }
      else if (_scene->forceWireframe) {
        // Wireframe render mode
        currenRenderMode = 1;
      }
      else if (_scene->forcePointsCloud()) {
        // Points cloud render mode
        currenRenderMode = 2;
      }
      int renderModeChoice = currenRenderMode;
      // Radio buttons
      ImGui::RadioButton("Solid", &renderModeChoice, 0);
      ImGui::RadioButton("Wireframe", &renderModeChoice, 1);
      ImGui::RadioButton("Point", &renderModeChoice, 2);
      // Handle render mode change
      if (renderModeChoice != currenRenderMode) {
        if (renderModeChoice == 0) {
          _scene->forceWireframe = false;
          _scene->setForcePointsCloud(false);
        }
        else if (renderModeChoice == 1) {
          _scene->forceWireframe = true;
          _scene->setForcePointsCloud(false);
        }
        else if (renderModeChoice == 2) {
          _scene->forceWireframe = false;
          _scene->setForcePointsCloud(true);
        }
      }
    }

    // Textures
    bool origValue = false, value = false;
    if (ImGui::CollapsingHeader("Texture Channels", "Texture Channels", true,
                                true)) {
      // Diffuse Texture
      {
        origValue = value = StandardMaterial::DiffuseTextureEnabled();
        ImGui::Checkbox("Diffuse Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetDiffuseTextureEnabled(value);
        }
      }
      // Ambient Texture
      {
        origValue = value = StandardMaterial::AmbientTextureEnabled();
        ImGui::Checkbox("Ambient Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetAmbientTextureEnabled(value);
        }
      }
      // Specular Texture
      {
        origValue = value = StandardMaterial::SpecularTextureEnabled();
        ImGui::Checkbox("Specular Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetSpecularTextureEnabled(value);
        }
      }
      // Emissive Texture
      {
        origValue = value = StandardMaterial::EmissiveTextureEnabled();
        ImGui::Checkbox("Emissive Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetEmissiveTextureEnabled(value);
        }
      }
      // Bump Texture
      {
        origValue = value = StandardMaterial::BumpTextureEnabled();
        ImGui::Checkbox("Bump Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetBumpTextureEnabled(value);
        }
      }
      // Opacity Texture
      {
        origValue = value = StandardMaterial::OpacityTextureEnabled();
        ImGui::Checkbox("Opacity Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetOpacityTextureEnabled(value);
        }
      }
      // Reflection Texture
      {
        origValue = value = StandardMaterial::ReflectionTextureEnabled();
        ImGui::Checkbox("Reflection Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetReflectionTextureEnabled(value);
        }
      }
      // Refraction Texture
      {
        origValue = value = StandardMaterial::RefractionTextureEnabled();
        ImGui::Checkbox("Refraction Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetRefractionTextureEnabled(value);
        }
      }
      // ColorGrading
      {
        origValue = value = StandardMaterial::ColorGradingTextureEnabled();
        ImGui::Checkbox("ColorGrading", &value);
        if (origValue != value) {
          StandardMaterial::SetColorGradingTextureEnabled(value);
        }
      }
      // Lightmap Texture
      {
        origValue = value = StandardMaterial::LightmapTextureEnabled();
        ImGui::Checkbox("Lightmap Texture", &value);
        if (origValue != value) {
          StandardMaterial::SetLightmapTextureEnabled(value);
        }
      }
      // Fresnel
      {
        origValue = value = StandardMaterial::FresnelEnabled();
        ImGui::Checkbox("Fresnel", &value);
        if (origValue != value) {
          StandardMaterial::SetFresnelEnabled(value);
        }
      }
    }
    // Options
    if (ImGui::CollapsingHeader("Options", "Options", true, true)) {
      // Animations
      {
        ImGui::Checkbox("Animations", &_scene->animationsEnabled);
      }
      // Collisions
      {
        ImGui::Checkbox("Collisions", &_scene->collisionsEnabled);
      }
      // Fog
      {
        origValue = value = _scene->fogEnabled();
        ImGui::Checkbox("Fog", &value);
        if (origValue != value) {
          _scene->setFogEnabled(value);
        }
      }
      // Lens Flares
      {
        ImGui::Checkbox("Lens Flares", &_scene->lensFlaresEnabled);
      }
      // Lights
      {
        origValue = value = _scene->lightsEnabled();
        ImGui::Checkbox("Lights", &value);
        if (origValue != value) {
          _scene->setLightsEnabled(value);
        }
      }
      // Particles
      {
        ImGui::Checkbox("Particles", &_scene->particlesEnabled);
      }
      // Post-processes
      {
        ImGui::Checkbox("Post-processes", &_scene->postProcessesEnabled);
      }
      // Probes
      {
        ImGui::Checkbox("Probes", &_scene->probesEnabled);
      }
      // Procedural Textures
      {
        ImGui::Checkbox("Procedural Textures",
                        &_scene->proceduralTexturesEnabled);
      }
      // Render Targets
      {
        ImGui::Checkbox("Render Targets", &_scene->renderTargetsEnabled);
      }
      // Shadows
      {
        origValue = value = _scene->shadowsEnabled();
        ImGui::Checkbox("Shadows", &value);
        if (origValue != value) {
          _scene->setShadowsEnabled(value);
        }
      }
      // Skeletons
      {
        origValue = value = _scene->skeletonsEnabled();
        ImGui::Checkbox("Skeletons", &value);
        if (origValue != value) {
          _scene->setSkeletonsEnabled(value);
        }
      }
      // Sprites
      {
        ImGui::Checkbox("Sprites", &_scene->spritesEnabled);
      }
      // Textures
      {
        origValue = value = _scene->texturesEnabled();
        ImGui::Checkbox("Textures", &value);
        if (origValue != value) {
          _scene->setTexturesEnabled(value);
        }
      }
    }
  }
  ImGui::EndDock();
}

void SceneTab::dispose()
{
}

} // end of namespace BABYLON
