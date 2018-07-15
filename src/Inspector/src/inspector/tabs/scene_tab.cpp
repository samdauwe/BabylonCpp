#include <babylon/inspector/tabs/scene_tab.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>

#include <babylon/inspector/inspector.h>
#include <babylon/inspector/properties/properties_view.h>

#include <babylon/imgui/imgui_dock.h>
#include <babylon/imgui/imgui_utils.h>

namespace BABYLON {

SceneTab::SceneTab(Inspector& inspector)
    : Tab{"Scene"}
    , _inspector{inspector}
    , _scene{inspector.scene()}
    , _splitterIntialized{false}
    , _width{0.f}
    , _sz1{0.f}
    , _sz2{0.f}
{
  // Build properties view
  _properties = ::std::make_unique<PropertiesView>();
  _buildPropertiesView();
}

SceneTab::~SceneTab()
{
}

void SceneTab::render()
{
  if (!_scene) {
    return;
  }

  if (!_splitterIntialized) {
    _width = ImGui::GetContentRegionMax().x;
    _sz1   = ImGui::GetContentRegionMax().y / 2.f;
    _sz2   = ImGui::GetContentRegionMax().y / 2.f;
  }

  if (ImGui::BeginDock(name.c_str())) {
    ImGui::Splitter(false, 1.0f, &_sz1, &_sz2, 8, 8, _width);
    // Render the actions panel
    if (ImGui::BeginChild("top", ImVec2(_width, _sz1), true)) {
      _renderActions();
    }
    ImGui::EndChild();
    // Render the properties panel
    if (ImGui::BeginChild("bottom", ImVec2(_width, _sz2), true)) {
      _renderProperties();
    }
    ImGui::EndChild();
  }
  ImGui::EndDock();
}

void SceneTab::dispose()
{
}

void SceneTab::_buildPropertiesView()
{
  // -- Add properties -- //
  auto& view = *_properties;
  /** Scene properties **/
  // - ambientColor
  view.addColor3Property(
    "ambientColor", [&]() -> Color3 const& { return _scene->ambientColor; },
    [&](const Color3& value) { _scene->ambientColor = value; });
  // - animationsEnabled
  view.addBoolProperty(
    "animationsEnabled", [&]() -> bool { return _scene->animationsEnabled; },
    [&](const bool& value) { _scene->animationsEnabled = value; });
  // - animationTimeScale
  view.addSizeTProperty(
    "animationTimeScale",
    [&]() -> size_t { return _scene->animationTimeScale; },
    [&](const size_t& value) { _scene->animationTimeScale = value; });
  // - audioEnabled
  view.addBoolProperty(
    "audioEnabled", [&]() -> bool { return _scene->audioEnabled(); },
    [&](const bool& value) { _scene->audioEnabled = value; });
  // - autoClear
  view.addBoolProperty("autoClear", [&]() -> bool { return _scene->autoClear; },
                       [&](const bool& value) { _scene->autoClear = value; });
  // - autoClearDepthAndStencil
  view.addBoolProperty(
    "autoClearDepthAndStencil",
    [&]() -> bool { return _scene->autoClearDepthAndStencil; },
    [&](const bool& value) { _scene->autoClearDepthAndStencil = value; });
  // - clearColor
  view.addColor4Property(
    "clearColor", [&]() -> Color4 const& { return _scene->clearColor; },
    [&](const Color4& value) { _scene->clearColor = value; });
  // - collisionsEnabled
  view.addBoolProperty(
    "collisionsEnabled", [&]() -> bool { return _scene->collisionsEnabled; },
    [&](const bool& value) { _scene->collisionsEnabled = value; });
  // - constantlyUpdateMeshUnderPointer
  view.addBoolProperty(
    "constantlyUpdateMeshUnderPointer",
    [&]() -> bool { return _scene->constantlyUpdateMeshUnderPointer; },
    [&](const bool& value) {
      _scene->constantlyUpdateMeshUnderPointer = value;
    });
  // - defaultMaterial
  view.addStringProperty(
    "defaultMaterial",
    [&]() -> string_t& { return _scene->defaultMaterial()->name; },
    [&](const string_t& /*value*/) {});
  // - dispatchAllSubMeshesOfActiveMeshes
  view.addBoolProperty(
    "dispatchAllSubMeshesOfActiveMeshes",
    [&]() -> bool { return _scene->dispatchAllSubMeshesOfActiveMeshes; },
    [&](const bool& value) {
      _scene->dispatchAllSubMeshesOfActiveMeshes = value;
    });
  // - dumpNextRenderTargets
  view.addBoolProperty(
    "dumpNextRenderTargets",
    [&]() -> bool { return _scene->dumpNextRenderTargets; },
    [&](const bool& value) { _scene->dumpNextRenderTargets = value; });
  // - fogColor
  view.addColor3Property(
    "fogColor", [&]() -> Color3 const& { return _scene->fogColor; },
    [&](const Color3& value) { _scene->fogColor = value; });
  // - fogDensity
  view.addFloatProperty(
    "fogDensity", [&]() -> float { return _scene->fogDensity; },
    [&](const float& value) { _scene->fogDensity = value; });
  // - fogEnabled
  view.addBoolProperty("fogEnabled",
                       [&]() -> bool { return _scene->fogEnabled(); },
                       [&](const bool& value) { _scene->fogEnabled = value; });
  // - fogEnd
  view.addFloatProperty("fogEnd", [&]() -> float { return _scene->fogEnd; },
                        [&](const float& value) { _scene->fogEnd = value; });
  // - fogMode
  view.addSizeTProperty("fogMode",
                        [&]() -> size_t { return _scene->fogMode(); },
                        [&](const size_t& value) {
                          _scene->fogMode = static_cast<unsigned>(value);
                        });
  // - fogStart
  view.addFloatProperty("fogStart", [&]() -> float { return _scene->fogStart; },
                        [&](const float& value) { _scene->fogStart = value; });
  // - forcePointsCloud
  view.addBoolProperty(
    "forcePointsCloud", [&]() -> bool { return _scene->forcePointsCloud(); },
    [&](const bool& value) { _scene->forcePointsCloud = value; });
  // - forceShowBoundingBoxes
  view.addBoolProperty(
    "forceShowBoundingBoxes",
    [&]() -> bool { return _scene->forceShowBoundingBoxes; },
    [&](const bool& value) { _scene->forceShowBoundingBoxes = value; });
  // - forceWireframe
  view.addBoolProperty(
    "forceWireframe", [&]() -> bool { return _scene->forceWireframe(); },
    [&](const bool& value) { _scene->forceWireframe = value; });
  // - gravity
  view.addVector3Property(
    "gravity", [&]() -> Vector3 const& { return _scene->gravity; },
    [&](const Vector3& value) { _scene->gravity = value; });
  // - headphone
  view.addBoolProperty("headphone",
                       [&]() -> bool { return _scene->headphone(); },
                       [&](const bool& value) { _scene->headphone = value; });
  // - hoverCursor
  view.addStringProperty(
    "hoverCursor", [&]() -> string_t& { return _scene->hoverCursor; },
    [&](const string_t& value) { _scene->hoverCursor = value; });
  // - imageProcessingConfiguration
  if (_scene->imageProcessingConfiguration()) {
    view.addImageProcessingConfigurationProperty(
      "imageProcessingConfiguration", [&]() -> ImageProcessingConfiguration& {
        return *_scene->imageProcessingConfiguration();
      });
  }
  // - isDisposed
  view.addBoolProperty("isDisposed",
                       [&]() -> bool { return _scene->isDisposed(); },
                       [&](const bool& /*value*/) {});
  // - isLoading
  view.addBoolProperty("isLoading",
                       [&]() -> bool { return _scene->isLoading(); },
                       [&](const bool& /*value*/) {});
  // - lensFlaresEnabled
  view.addBoolProperty(
    "lensFlaresEnabled", [&]() -> bool { return _scene->lensFlaresEnabled; },
    [&](const bool& value) { _scene->lensFlaresEnabled = value; });
  // - lightsEnabled
  view.addBoolProperty(
    "lightsEnabled", [&]() -> bool { return _scene->lightsEnabled(); },
    [&](const bool& value) { _scene->lightsEnabled = value; });
  // - particlesEnabled
  view.addBoolProperty(
    "particlesEnabled", [&]() -> bool { return _scene->particlesEnabled; },
    [&](const bool& value) { _scene->particlesEnabled = value; });
  // - pointerX
  view.addIntProperty("pointerX", [&]() -> int { return _scene->pointerX(); },
                      [&](const int& /*value*/) {});
  // - pointerY
  view.addIntProperty("pointerY", [&]() -> int { return _scene->pointerY(); },
                      [&](const int& /*value*/) {});
  // - postProcessesEnabled
  view.addBoolProperty(
    "postProcessesEnabled",
    [&]() -> bool { return _scene->postProcessesEnabled; },
    [&](const bool& value) { _scene->postProcessesEnabled = value; });
  // - preventDefaultOnPointerDown
  view.addBoolProperty(
    "preventDefaultOnPointerDown",
    [&]() -> bool { return _scene->preventDefaultOnPointerDown; },
    [&](const bool& value) { _scene->preventDefaultOnPointerDown = value; });
  // - probesEnabled
  view.addBoolProperty(
    "probesEnabled", [&]() -> bool { return _scene->probesEnabled; },
    [&](const bool& value) { _scene->probesEnabled = value; });
  // - proceduralTexturesEnabled
  view.addBoolProperty(
    "proceduralTexturesEnabled",
    [&]() -> bool { return _scene->proceduralTexturesEnabled; },
    [&](const bool& value) { _scene->proceduralTexturesEnabled = value; });
  // - renderTargetsEnabled
  view.addBoolProperty(
    "renderTargetsEnabled",
    [&]() -> bool { return _scene->renderTargetsEnabled; },
    [&](const bool& value) { _scene->renderTargetsEnabled = value; });
  // - requireLightSorting
  view.addBoolProperty(
    "requireLightSorting",
    [&]() -> bool { return _scene->requireLightSorting; },
    [&](const bool& value) { _scene->requireLightSorting = value; });
  // - shadowsEnabled
  view.addBoolProperty(
    "shadowsEnabled", [&]() -> bool { return _scene->shadowsEnabled(); },
    [&](const bool& value) { _scene->shadowsEnabled = value; });
  // - skeletonsEnabled
  view.addBoolProperty(
    "skeletonsEnabled", [&]() -> bool { return _scene->skeletonsEnabled(); },
    [&](const bool& value) { _scene->skeletonsEnabled = value; });
  // - spritesEnabled
  view.addBoolProperty(
    "spritesEnabled", [&]() -> bool { return _scene->spritesEnabled; },
    [&](const bool& value) { _scene->spritesEnabled = value; });
  // - texturesEnabled
  view.addBoolProperty(
    "texturesEnabled", [&]() -> bool { return _scene->texturesEnabled(); },
    [&](const bool& value) { _scene->texturesEnabled = value; });
  // - uid
  view.addStringProperty("uid", [&]() -> string_t { return _scene->uid(); },
                         [&](const string_t& /*value*/) {});
  // - unTranslatedPointer
  view.addVector2Property(
    "unTranslatedPointer",
    [&]() -> Vector2 const& { return _scene->unTranslatedPointer(); },
    [&](const Vector2& /*value*/) {});
  // - useConstantAnimationDeltaTime
  view.addBoolProperty(
    "useConstantAnimationDeltaTime",
    [&]() -> bool { return _scene->useConstantAnimationDeltaTime; },
    [&](const bool& value) { _scene->useConstantAnimationDeltaTime = value; });
  // - useRightHandedSystem
  view.addBoolProperty(
    "useRightHandedSystem",
    [&]() -> bool { return _scene->useRightHandedSystem(); },
    [&](const bool& value) { _scene->useRightHandedSystem = value; });
  // - workerCollisions
  view.addBoolProperty(
    "workerCollisions", [&]() -> bool { return _scene->workerCollisions(); },
    [&](const bool& value) { _scene->workerCollisions = value; });
  // -- Sort properties by property name -- //
  view.sortPropertiesByName();
}

void SceneTab::_renderActions()
{
  // General
  if (ImGui::CollapsingHeader("General", "General", true, true)) {
    ImGui::Checkbox("Bounding boxes", &_scene->forceShowBoundingBoxes);
  }
  // Rendering Mode
  if (ImGui::CollapsingHeader("Rendering Mode", "Rendering Mode", true, true)) {
    // Determine current render mode
    int currenRenderMode = -1;
    if (_scene->forcePointsCloud()) {
      // Points cloud render mode
      currenRenderMode = 0;
    }
    else if (_scene->forceWireframe()) {
      // Wireframe render mode
      currenRenderMode = 1;
    }
    else if (!_scene->forceWireframe() && !_scene->forcePointsCloud()) {
      // Solid render mode
      currenRenderMode = 2;
    }
    int renderModeChoice = currenRenderMode;
    // Radio buttons
    ImGui::RadioButton("Point", &renderModeChoice, 0);
    ImGui::RadioButton("Wireframe", &renderModeChoice, 1);
    ImGui::RadioButton("Solid", &renderModeChoice, 2);
    // Handle render mode change
    if (renderModeChoice != currenRenderMode) {
      if (renderModeChoice == 0) {
        _scene->forceWireframe   = false;
        _scene->forcePointsCloud = true;
      }
      else if (renderModeChoice == 1) {
        _scene->forceWireframe   = true;
        _scene->forcePointsCloud = false;
      }
      else if (renderModeChoice == 2) {
        _scene->forceWireframe   = false;
        _scene->forcePointsCloud = false;
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
        _scene->fogEnabled = value;
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
        _scene->lightsEnabled = value;
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
        _scene->shadowsEnabled = value;
      }
    }
    // Skeletons
    {
      origValue = value = _scene->skeletonsEnabled();
      ImGui::Checkbox("Skeletons", &value);
      if (origValue != value) {
        _scene->skeletonsEnabled = value;
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
        _scene->texturesEnabled = value;
      }
    }
  }
}

void SceneTab::_renderProperties()
{
  if (_properties) {
    _properties->render();
  }
}

} // end of namespace BABYLON
