#ifndef BABYLON_DEBUG_ISKELETON_VIEWER_H
#define BABYLON_DEBUG_ISKELETON_VIEWER_H

#include <optional>

namespace BABYLON {

/**
 * @brief Defines how to display the various bone meshes for the viewer.
 */
struct ISkeletonViewerDisplayOptions {
  /** How far down to start tapering the bone spurs */
  std::optional<float> midStep = std::nullopt;

  /** How big is the midStep? */
  std::optional<float> midStepFactor = std::nullopt;

  /** Base for the Sphere Size */
  std::optional<float> sphereBaseSize = std::nullopt;

  /** The ratio of the sphere to the longest bone in units */
  std::optional<float> sphereScaleUnit = std::nullopt;

  /** Ratio for the Sphere Size */
  std::optional<float> sphereFactor = std::nullopt;
}; // end of struct ISkeletonViewerDisplayOptions

/**
 * @brief Defines the options associated with the creation of a SkeletonViewer.
 */
struct ISkeletonViewerOptions {
  /** Should the system pause animations before building the Viewer? */
  std::optional<bool> pauseAnimations = std::nullopt;

  /** Should the system return the skeleton to rest before building? */
  std::optional<bool> returnToRest = std::nullopt;

  /** public Display Mode of the Viewer */
  std::optional<unsigned int> displayMode = std::nullopt;

  /** Flag to toggle if the Viewer should use the CPU for animations or not? */
  ISkeletonViewerDisplayOptions displayOptions;

  /** Flag to toggle if the Viewer should use the CPU for animations or not? */
  std::optional<bool> computeBonesUsingShaders = std::nullopt;
}; // end of struct ISkeletonViewerOptions

} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_ISKELETON_VIEWER_H
