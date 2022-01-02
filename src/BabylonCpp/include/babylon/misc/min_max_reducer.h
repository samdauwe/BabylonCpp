#ifndef BABYLON_MISC_MIN_MAX_REDUCER_H
#define BABYLON_MISC_MIN_MAX_REDUCER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/structs.h>
#include <babylon/engines/constants.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

FWD_CLASS_SPTR(Camera)
FWD_CLASS_SPTR(PostProcess)
FWD_CLASS_SPTR(PostProcessManager)
FWD_CLASS_SPTR(RenderTargetTexture)

/**
 * @brief This class computes a min/max reduction from a texture: it means it computes the minimum
 * and maximum values from all values of the texture.
 * It is performed on the GPU for better performances, thanks to a succession of post processes.
 * The source values are read from the red channel of the texture.
 */
class BABYLON_SHARED_EXPORT MinMaxReducer {

public:
  /**
   * @brief Creates a min/max reducer.
   * @param camera The camera to use for the post processes
   */
  MinMaxReducer(const CameraPtr& camera);
  virtual ~MinMaxReducer(); // = default

  /**
   * @brief Sets the source texture to read the values from.
   * One must indicate if the texture is a depth texture or not through the depthRedux parameter
   * because in such textures '1' value must not be taken into account to compute the maximum
   * as this value is used to clear the texture.
   * Note that the computation is not activated by calling this function, you must call activate()
   * for that!
   * @param sourceTexture The texture to read the values from. The values should be in the red
   * channel.
   * @param depthRedux Indicates if the texture is a depth texture or not
   * @param type The type of the textures created for the reduction (defaults to
   * TEXTURETYPE_HALF_FLOAT)
   * @param forceFullscreenViewport Forces the post processes used for the reduction to be applied
   * without taking into account viewport (defaults to true)
   */
  virtual void setSourceTexture(const RenderTargetTexturePtr& sourceTexture, bool depthRedux,
                                unsigned int type            = Constants::TEXTURETYPE_HALF_FLOAT,
                                bool forceFullscreenViewport = true);

  /**
   * @brief Activates the reduction computation.
   * When activated, the observers registered in onAfterReductionPerformed are
   * called after the computation is performed
   */
  virtual void activate();

  /**
   * @brief Deactivates the reduction computation.
   */
  virtual void deactivate();

  /**
   * @brief Disposes the min/max reducer.
   * @param disposeAll true to dispose all the resources. You should always call this function with
   * true as the parameter (or without any parameter as it is the default one). This flag is meant
   * to be used internally.
   */
  virtual void dispose(bool disposeAll = true);

protected:
  /**
   * @brief Gets the texture used to read the values from.
   */
  RenderTargetTexturePtr& get_sourceTexture();

  /**
   * @brief Gets the refresh rate of the computation.
   * Use 0 to compute just once, 1 to compute on every frame, 2 to compute every two frames and so
   * on...
   */
  int get_refreshRate() const;

  /**
   * @brief Sets the refresh rate of the computation.
   * Use 0 to compute just once, 1 to compute on every frame, 2 to compute every two frames and so
   * on...
   */
  void set_refreshRate(int value);

  /**
   * @brief Gets the activation status of the reducer.
   */
  bool get_activated() const;

public:
  /**
   * Observable triggered when the computation has been performed
   */
  Observable<MinMaxFloats> onAfterReductionPerformed;

  /**
   * Defines the refresh rate of the computation.
   * Use 0 to compute just once, 1 to compute on every frame, 2 to compute every two frames and so
   * on...
   */
  Property<MinMaxReducer, int> refreshRate;

  /**
   * Gets the activation status of the reducer
   */
  ReadOnlyProperty<MinMaxReducer, bool> activated;

protected:
  CameraPtr _camera;
  RenderTargetTexturePtr _sourceTexture;
  std::vector<PostProcessPtr> _reductionSteps;
  std::unique_ptr<PostProcessManager> _postProcessManager;
  Observer<RenderTargetTexture>::Ptr _onAfterUnbindObserver;
  bool _forceFullscreenViewport;
  bool _activated;

}; // end of class MinMaxReducer

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_MIN_MAX_REDUCER_H
