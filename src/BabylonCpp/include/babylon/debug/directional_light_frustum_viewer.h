#ifndef BABYLON_DEBUG_DIRECTIONAL_LIGHT_FRUSTRUM_VIEWER_H
#define BABYLON_DEBUG_DIRECTIONAL_LIGHT_FRUSTRUM_VIEWER_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/maths/matrix.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

class Scene;
FWD_CLASS_SPTR(Camera)
FWD_CLASS_SPTR(DirectionalLight)
FWD_CLASS_SPTR(Mesh)
FWD_CLASS_SPTR(TransformNode)

/**
 * @brief Class used to render a debug view of the frustum for a directional light.
 * @see https://playground.babylonjs.com/#7EFGSG#3
 */
class BABYLON_SHARED_EXPORT DirectionalLightFrustumViewer {

public:
  /**
   * @brief Creates a new frustum viewer.
   * @param light directional light to display the frustum for
   * @param camera camera used to retrieve the minZ / maxZ values if the shadowMinZ/shadowMaxZ
   * values of the light are not setup
   */
  DirectionalLightFrustumViewer(const DirectionalLightPtr& light, const CameraPtr& camera);
  ~DirectionalLightFrustumViewer(); // = default

  /**
   * @brief Shows the frustum.
   */
  void show();

  /**
   * @brief Hides the frustum.
   */
  void hide();

  /**
   * @brief Updates the frustum.
   * Call this method to update the frustum view if the light has changed position/direction
   */
  void update();

  /**
   * @brief Dispose of the class / remove the frustum view.
   */
  void dispose();

protected:
  float get_transparency() const;
  void set_transparency(float alpha);
  bool get_showLines() const;
  void set_showLines(bool show);
  bool get_showPlanes() const;
  void set_showPlanes(bool show);

  void _createGeometry();
  Matrix& _getInvertViewMatrix();

public:
  /**
   * Gets or sets the transparency of the frustum planes
   */
  Property<DirectionalLightFrustumViewer, float> transparency;

  /**
   * true to display the edges of the frustum
   */
  Property<DirectionalLightFrustumViewer, bool> showLines;

  /**
   * true to display the planes of the frustum
   */
  Property<DirectionalLightFrustumViewer, bool> showPlanes;

private:
  Scene* _scene;
  DirectionalLightPtr _light;
  CameraPtr _camera;
  Matrix _inverseViewMatrix;
  bool _visible;

  TransformNodePtr _rootNode;
  std::vector<MeshPtr> _lightHelperFrustumMeshes;

  std::vector<Vector3> _nearLinesPoints;
  std::vector<Vector3> _farLinesPoints;
  std::vector<Vector3> _trLinesPoints;
  std::vector<Vector3> _brLinesPoints;
  std::vector<Vector3> _tlLinesPoints;
  std::vector<Vector3> _blLinesPoints;

  Float32Array _nearPlaneVertices;
  Float32Array _farPlaneVertices;
  Float32Array _rightPlaneVertices;
  Float32Array _leftPlaneVertices;
  Float32Array _topPlaneVertices;
  Float32Array _bottomPlaneVertices;

  Vector3 _oldPosition;
  Vector3 _oldDirection;
  bool _oldAutoCalc;
  float _oldMinZ;
  float _oldMaxZ;

  float _transparency;
  bool _showLines;
  bool _showPlanes;

}; // end of class DirectionalLightFrustumViewer

} // end of namespace BABYLON

#endif // end of BABYLON_DEBUG_DIRECTIONAL_LIGHT_FRUSTRUM_VIEWER_H
