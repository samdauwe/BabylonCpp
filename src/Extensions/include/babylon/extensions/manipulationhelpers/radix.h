#ifndef BABYLON_EXTENSIONS_MANIPULATION_HELPERS_RADIX_H
#define BABYLON_EXTENSIONS_MANIPULATION_HELPERS_RADIX_H

#include <babylon/babylon_global.h>
#include <babylon/math/color3.h>

namespace BABYLON {
namespace Extensions {

enum class RadixFeatures : int {

  None = 0x0000,

  /**
   * Display the Arrow that follows the X Axis
   */
  ArrowX = 0x0001,

  /**
   * Display the Arrow that follows the Y Axis
   */
  ArrowY = 0x0002,

  /**
   * Display the Arrow that follows the Z Axis
   */
  ArrowZ = 0x0004,

  /**
   * Display the Arrow that follows the XYZ Axis
   */
  ArrowsXYZ = 0x0007,

  /**
   * Display the anchor that allow XY plane manipulation
   */
  PlaneSelectionXY = 0x0010,

  /**
   * Display the anchor that allow XZ plane manipulation
   */
  PlaneSelectionXZ = 0x0020,

  /**
   * Display the anchor that allow YZ plane manipulation
   */
  PlaneSelectionYZ = 0x0040,

  /**
   * Display all the anchors that allow plane manipulation
   */
  AllPlanesSelection = 0x0070,

  /**
   * Display the rotation cylinder that allows rotation manipulation along the
   * X Axis
   */
  RotationX = 0x0100,

  /**
   * Display the rotation cylinder that allows rotation manipulation along the
   * Y Axis
   */
  RotationY = 0x0200,

  /**
   * Display the rotation cylinder that allows rotation manipulation along the
   * A Axis
   */
  RotationZ = 0x0400,

  /**
   * Display all rotation cylinders
   */
  Rotations = 0x0700,

  /**
   * NOT SUPPORTED RIGHT NOW
   */
  CenterSquare = 0x1000

}; // end of enum class RadixFeatures

using T = std::underlying_type_t<RadixFeatures>;

inline constexpr bool operator!(RadixFeatures rhs)
{
  return (rhs == RadixFeatures::None);
}

inline constexpr RadixFeatures operator&(RadixFeatures lhs, RadixFeatures rhs)

{
  return static_cast<RadixFeatures>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline constexpr RadixFeatures& operator&=(RadixFeatures& lhs,
                                           RadixFeatures rhs)
{
  lhs = static_cast<RadixFeatures>(static_cast<T>(lhs) & static_cast<T>(rhs));
  return lhs;
}

inline constexpr RadixFeatures operator|(RadixFeatures lhs, RadixFeatures rhs)

{
  return static_cast<RadixFeatures>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline constexpr RadixFeatures& operator|=(RadixFeatures& lhs,
                                           RadixFeatures rhs)
{
  lhs = static_cast<RadixFeatures>(static_cast<T>(lhs) | static_cast<T>(rhs));
  return lhs;
}

/**
 * @brief This class create the visual geometry to display a manipulation radix
 * in a viewport. It also implements the logic to handler intersection, hover on
 * feature.
 */
class BABYLON_SHARED_EXPORT Radix {

public:
  /**
   * @brief Create a new Radix instance. The length/radius members are optionals
   * and the default value should suit most cases
   * @param scene the owner Scene
   * @param features the feature the radix must display
   * @param arrowLength the length of a row of an axis, include the rotation
   * cylinder (if any), but always exclude the arrow cone
   * @param coneLength the length of the arrow cone. this is also the length
   * taken for the rotation cylinder (if any)
   * @param coneRadius the radius of the arrow cone
   * @param planeSelectionLength the length of the selection plane
   */
  Radix(Scene* scene,
        RadixFeatures features = RadixFeatures::ArrowsXYZ
                                 | RadixFeatures::AllPlanesSelection
                                 | RadixFeatures::Rotations,
        float arrowLength = 1.f, float coneLength = 0.2f,
        float coneRadius = 0.1f, float planeSelectionLength = 0.2f);
  ~Radix();

  /**
   * @brief Make an intersection test between a point position in the viwport
   * and the Radix, return the feature that is intersected, if any. Only the
   * closer Radix Feature is picked.
   * @param pos the viewport position to create the picking ray from.
   * @return the intersection test
   */
  RadixFeatures intersect(const Vector2& pos);

  /**
   * @brief Set the world coordinate of where the Axis should be displayed
   * @param position the position
   * @param rotation the rotation quaternion
   * @param scale the scale (should be uniform)
   */
  void setWorld(const Vector3& position, const Quaternion& rotation,
                const Vector3& scale);

  /**
   * @brief Display the Radix on screen.
   */
  void show();

  /**
   * @brief Hide the Radix from the screen.
   */
  void hide();

  /** Getters / Setters **/

  /**
   * @brief Gets the Wire Selection Threshold, set a bigger value to improve
   * tolerance while picking a wire mesh.
   * @return the Wire Selection Threshold.
   */
  float wireSelectionThreshold();

  /**
   * @brief Sets the Wire Selection Threshold, set a bigger value to improve
   * tolerance while picking a wire mesh.
   * @param value the Wire Selection Threshold.
   */
  void setWireSelectionThreshold(float value);

  /**
   * @brief Gets the color of the X Arrow.
   * @return the color of the X Arrow.
   */
  Color3& xArrowColor();

  /**
   * @brief Sets the color of the X Arrow.
   * @param value the color of the X Arrow.
   */
  void setXArrowColor(const Color3& value);

  /**
   * @brief Gets the color of the Y Arrow.
   * @return the color of the Y Arrow.
   */
  Color3& yArrowColor();

  /**
   * @brief Sets the color of the Y Arrow.
   * @param value the color of the Y Arrow.
   */
  void setYArrowColor(const Color3& value);

  /**
   * @brief Gets the color of the Z Arrow.
   * @return the color of the Z Arrow.
   */
  Color3& zArrowColor();

  /**
   * @brief Sets the color of the Z Arrow.
   * @param value the color of the Z Arrow.
   */
  void setZArrowColor(const Color3& value);

  /**
   * @brief Gets the color of the XY Plane selection anchor.
   * @return the color of the XY Plane selection anchor.
   */
  Color3& xyPlaneSelectionColor();

  /**
   * @brief Sets the color of the XY Plane selection anchor.
   * @param the color of the XY Plane selection anchor.
   */
  void setXYPlaneSelectionColor(const Color3& value);

  /**
   * @brief Gets the color of the XZ Plane selection anchor.
   * @return the color of the XZ Plane selection anchor.
   */
  Color3& xzPlaneSelectionColor();

  /**
   * @brief Sets the color of the XZ Plane selection anchor.
   * @param the color of the XZ Plane selection anchor.
   */
  void setXZPlaneSelectionColor(const Color3& value);

  /**
   * @brief Gets the color of the YZ Plane selection anchor.
   * @return the color of the YZ Plane selection anchor.
   */
  Color3& yzPlaneSelectionColor();

  /**
   * @brief Sets the color of the YZ Plane selection anchor.
   * @param the color of the YZ Plane selection anchor.
   */
  void setYZPlaneSelectionColor(const Color3& value);

  /**
   * @brief Gets the feature of the Radix that are/must be highlighted.
   * @return the feature of the Radix that are/must be highlighted.
   */
  RadixFeatures highlighted() const;

  /**
   * @brief Sets the feature of the Radix that are/must be highlighted.
   * @param value the feature of the Radix that are/must be highlighted.
   */
  void setHighlighted(RadixFeatures value);

  /**
   * @brief Gets the Radix Features that were selected upon creation.
   * @return
   */
  RadixFeatures features() const;

private:
  void setVisibleState(AbstractMesh* mesh, bool state);
  float intersectMeshes(const Vector2& pos, const std::string& startName,
                        float currentClosest);
  void constructGraphicalObjects();
  void constructArrow(RadixFeatures feature, const std::string& name,
                      const Matrix& transform);
  void constructPlaneSelection(RadixFeatures feature, const std::string& name,
                               const Matrix& transform);
  void constructRotation(RadixFeatures feature, const std::string& name,
                         const Matrix& transform);
  void addSymbolicMeshToLit(AbstractMesh* mesh);
  bool hasFeature(RadixFeatures value);
  bool hasHighlightedFeature(RadixFeatures value);
  void updateMaterial(const std::string& name, const Color3& color);
  void updateMaterialFromHighlighted(RadixFeatures feature,
                                     RadixFeatures highlighted,
                                     const std::string& name);
  StandardMaterial* getMaterial(const std::string& name);

private:
  static float pc;
  static float sc;

private:
  float _arrowLength;
  float _coneLength;
  float _coneRadius;
  float _planeSelectionLength;
  PointLight* _light1;
  PointLight* _light2;
  Mesh* _rootMesh;
  RadixFeatures _features;
  Scene* _scene;
  std::unordered_map<std::string, Material*> _materials;
  float _wireSelectionThreshold;
  Color3 _xArrowColor;
  Color3 _yArrowColor;
  Color3 _zArrowColor;
  Color3 _xyPlaneSelectionColor;
  Color3 _xzPlaneSelectionColor;
  Color3 _yzPlaneSelectionColor;
  RadixFeatures _highlighted;

}; // end of class Radix

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of
       // BABYLON_EXTENSIONS_MANIPULATION_HELPERS_RADIX_H
