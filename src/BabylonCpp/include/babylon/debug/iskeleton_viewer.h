#ifndef BABYLON_DEBUG_ISKELETON_VIEWER_H
#define BABYLON_DEBUG_ISKELETON_VIEWER_H

#include <optional>

#include <babylon/babylon_api.h>
#include <babylon/maths/color3.h>

namespace BABYLON {

class Skeleton;
using SkeletonPtr = std::shared_ptr<Skeleton>;

/**
 * @brief Defines how to display the various bone meshes for the viewer.
 */
struct BABYLON_SHARED_EXPORT ISkeletonViewerDisplayOptions {
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

  /** Whether a spur should attach its far end to the child bone position */
  std::optional<bool> spurFollowsChild = std::nullopt;

  /** Whether to show local axes or not  */
  std::optional<bool> showLocalAxes = std::nullopt;

  /** Length of each local axis */
  std::optional<float> localAxesSize = std::nullopt;
}; // end of struct ISkeletonViewerDisplayOptions

/**
 * @brief Defines the options associated with the creation of a SkeletonViewer.
 */
struct BABYLON_SHARED_EXPORT ISkeletonViewerOptions {
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

  /** Flag ignore non weighted bones */
  std::optional<bool> useAllBones = std::nullopt;
}; // end of struct ISkeletonViewerOptions

/**
 * @brief Defines the constructor options for the BoneWeight Shader.
 */
struct BABYLON_SHARED_EXPORT IBoneWeightShaderOptions {
  /** Skeleton to Map */
  SkeletonPtr skeleton = nullptr;

  /** Colors for Uninfluenced bones */
  std::optional<Color3> colorBase = std::nullopt;

  /** Colors for 0.0-0.25 Weight bones */
  std::optional<Color3> colorZero = std::nullopt;

  /** Color for 0.25-0.5 Weight Influence */
  std::optional<Color3> colorQuarter = std::nullopt;

  /** Color for 0.5-0.75 Weight Influence */
  std::optional<Color3> colorHalf = std::nullopt;

  /** Color for 0.75-1 Weight Influence */
  std::optional<Color3> colorFull = std::nullopt;

  /** Color for Zero Weight Influence */
  std::optional<float> targetBoneIndex = std::nullopt;
}; // end of struct IBoneWeightShaderOptions

/**
 * @brief Simple structure of the gradient steps for the Color Map.
 */
struct BABYLON_SHARED_EXPORT ISkeletonMapShaderColorMapKnot {
  /** Color of the Knot */
  std::optional<Color3> color = std::nullopt;
  /** Location of the Knot */
  std::optional<float> location = std::nullopt;
}; // end of struct ISkeletonMapShaderColorMapKnot

/**
 * @brief Defines the constructor options for the SkeletonMap Shader.
 */
struct BABYLON_SHARED_EXPORT ISkeletonMapShaderOptions {
  /** Skeleton to Map */
  SkeletonPtr skeleton = nullptr;
  /** Array of ColorMapKnots that make the gradient must be ordered with knot[i].location <
   * knot[i+1].location*/
  std::optional<std::vector<ISkeletonMapShaderColorMapKnot>> colorMap = std::nullopt;
}; // end of struct ISkeletonMapShaderOptions

} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_ISKELETON_VIEWER_H
