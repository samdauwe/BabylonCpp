#ifndef BABYLON_RENDERING_PRE_PASS_RENDERER_H
#define BABYLON_RENDERING_PRE_PASS_RENDERER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/color4.h>

namespace BABYLON {

class Effect;
class Engine;
class Scene;
FWD_CLASS_SPTR(AbstractMesh)
FWD_CLASS_SPTR(ImageProcessingPostProcess)
FWD_CLASS_SPTR(Material)
FWD_CLASS_SPTR(MultiRenderTarget)
FWD_CLASS_SPTR(PostProcess)
FWD_STRUCT_SPTR(PrePassEffectConfiguration)
FWD_CLASS_SPTR(SubSurfaceConfiguration)

struct TextureFormatMapping {
  unsigned int type   = 0;
  unsigned int format = 0;
}; // end of struct TextureFormatMapping

/**
 * @brief Renders a pre pass of the scene.
 * This means every mesh in the scene will be rendered to a render target texture
 * And then this texture will be composited to the rendering canvas with post processes
 * It is necessary for effects like subsurface scattering or deferred shading
 */
class BABYLON_SHARED_EXPORT PrePassRenderer : public std::enable_shared_from_this<PrePassRenderer> {

public:
  /**
   * @brief Instanciates a prepass renderer.
   * @param scene The scene
   */
  PrePassRenderer(Scene* scene);
  ~PrePassRenderer() = default;

  /**
   * @brief Sets the proper output textures to draw in the engine.
   * @param effect The effect that is drawn. It can be or not be compatible with drawing to several
   * output textures.
   */
  void bindAttachmentsForEffect(Effect& effect);

  /**
   * @brief Hidden
   */
  void _beforeCameraDraw();

  /**
   * @brief Hidden
   */
  void _afterCameraDraw();

  /**
   * @brief Clears the scene render target (in the sense of settings pixels to the scene clear color
   * value).
   */
  void clear();

  /**
   * @brief Adds an effect configuration to the prepass.
   * If an effect has already been added, it won't add it twice and will return the configuration
   * already present.
   * @param cfg the effect configuration
   * @return the effect configuration now used by the prepass
   */
  PrePassEffectConfigurationPtr addEffectConfiguration(const PrePassEffectConfigurationPtr& cfg);

  /**
   * @brief Returns the index of a texture in the multi render target texture array.
   * @param type Texture type
   * @return The index
   */
  unsigned int getIndex(unsigned int type);

  /**
   * @brief Marks the prepass renderer as dirty, triggering a check if the prepass is necessary for
   * the next rendering.
   */
  void markAsDirty();

  /**
   * @brief Disposes the prepass renderer.
   */
  void dispose();

protected:
  /**
   * Configuration for prepass effects
   */
  std::vector<PrePassEffectConfigurationPtr> _effectConfigurations;

  /**
   * @brief Indicates if the prepass is enabled.
   */
  bool get_enabled() const;

  /**
   * @brief Gets How many samples are used for MSAA of the scene render target.
   */
  unsigned int get_samples() const;

  /**
   * @brief Sets How many samples are used for MSAA of the scene render target.
   */
  void set_samples(unsigned int n);

  /**
   * @brief Indicates if rendering a prepass is supported.
   */
  bool get_isSupported() const;

private:
  void _initializeAttachments();
  void _createCompositionEffect();
  void _checkRTSize();
  void _bindFrameBuffer();
  void _setState(bool enabled);
  void _enable();
  void _disable();
  void _resetPostProcessChain();
  void _bindPostProcessChain();
  void _update();

public:
  /**
   * To save performance, we can excluded skinned meshes from the prepass
   */
  std::vector<AbstractMeshPtr> excludedSkinnedMesh;

  /**
   * Force material to be excluded from the prepass
   * Can be useful when `useGeometryBufferFallback` is set to `true`
   * and you don't want a material to show in the effect.
   */
  std::vector<MaterialPtr> excludedMaterials;

  /**
   * Number of textures in the multi render target texture where the scene is directly rendered
   */
  size_t mrtCount;

  /**
   * The render target where the scene is directly rendered
   */
  MultiRenderTargetPtr prePassRT;

  /**
   * Image processing post process for composition
   */
  ImageProcessingPostProcessPtr imageProcessingPostProcess;

  /**
   * Configuration for sub surface scattering post process
   */
  SubSurfaceConfigurationPtr subSurfaceConfiguration;

  /**
   * Should materials render their geometry on the MRT
   */
  bool materialsShouldRenderGeometry;

  /**
   * Should materials render the irradiance information on the MRT
   */
  bool materialsShouldRenderIrradiance;

  /**
   * Indicates if the prepass is enabled
   */
  ReadOnlyProperty<PrePassRenderer, bool> enabled;

  /**
   * How many samples are used for MSAA of the scene render target
   */
  Property<PrePassRenderer, unsigned int> samples;

  /**
   * Indicates if rendering a prepass is supported
   */
  ReadOnlyProperty<PrePassRenderer, bool> isSupported;

private:
  static std::vector<TextureFormatMapping> _textureFormats;
  IndicesArray _textureIndices;
  Scene* _scene;
  Engine* _engine;
  bool _isDirty;
  Uint32Array _multiRenderAttachments;
  Uint32Array _defaultAttachments;
  Uint32Array _clearAttachments;

  std::vector<PostProcessPtr> _postProcesses;

  const Color4 _clearColor;
  bool _enabled;

}; // end of class PrePassRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_RENDERING_PRE_PASS_RENDERER_H
