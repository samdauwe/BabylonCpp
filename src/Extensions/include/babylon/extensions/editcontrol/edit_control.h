#ifndef BABYLON_EXTENSIONS_EDIT_CONTROL_EDIT_CONTROL_H
#define BABYLON_EXTENSIONS_EDIT_CONTROL_EDIT_CONTROL_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/extensions/editcontrol/act_history.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/tools/event_state.h>

namespace BABYLON {
namespace Extensions {

class EditControl {

public:
  EditControl(Mesh* mesh, Camera* camera, ICanvas* canvas, float scale = 1.f);
  ~EditControl();

  void switchTo(Mesh* mesh);
  void setUndoCount(int c);
  void undo();
  void redo();
  void detach();
  void disposeAll();
  bool isEditing() const;
  bool isPointerOver() const;
  bool isTranslationEnabled() const;
  void enableTranslation();
  void disableTranslation();
  bool isRotationEnabled() const;
  void returnEuler(bool euler);
  void enableRotation();
  void disableRotation();
  bool isScalingEnabled() const;
  void enableScaling();
  void disableScaling();
  void setLocal(bool l);
  bool isLocal() const;
  void setTransSnap(bool s);
  void setRotSnap(bool s);
  void setScaleSnap(bool s);
  void setTransSnapValue(float t);
  void setRotSnapValue(float r);
  void setScaleSnapValue(float r);

private:
  void renderLoopProcess();
  void onPointerDown(PointerEvent& event);
  void detachControl(Camera* cam, ICanvas* can);
  void onPointerOver();
  void restoreColor(Mesh* mesh);
  void onPointerUp(const Event& event);
  void onPointerMove(const Event& event);
  Mesh* getPickPlane(Mesh* axis);
  void doTranslation(const Vector3& diff);
  void transWithSnap(Mesh* mesh, Vector3& trans, bool local);
  void doScaling(const Vector3& diff);
  void scaleWithSnap(Mesh* mesh, Vector3& p);
  void doRotation(Mesh* mesh, Mesh* axis, const Vector3& newPos);
  std::tuple<Vector3, bool> getPosOnPickPlane();
  void hideBaxis();
  /**
   * @brief Create axeses which will be shown in all three modes, translate,
   * rotate and scale.
   */
  void createGuideAxes();
  void createPickPlanes();
  void createTransAxes();
  Mesh* createTriangle(const std::string& name, float w, Scene* scene) const;
  void createRotAxes();
  Mesh* extrudeBox(float w, float l) const;
  LinesMesh* createCircle(float r, int t = 360) const;
  Mesh* createTube(float r, int t = 360) const;
  void createScaleAxes();
  /**
   * @brief this would be call during rotation as the local axes direction owuld
   * have changed
   * need to set the local axis.
   * These are used in all three modes to figure out direction of mouse move wrt
   * the axes
   * TODO should use world pivotmatrix instead of worldmatrix - incase pivot
   * axes were rotated?
   */
  void setLocalAxes(Mesh* mesh);
  void setAxesScale();
  static float getAngle(const Vector3& p1, const Vector3& p2, const Vector3& p,
                        const Vector3& cN);
  void createMaterials(Scene* scene);
  void disposeMaterials();
  static StandardMaterial* getStandardMaterial(const std::string& name,
                                               const Color3& col, Scene* scene);

private:
  Mesh* mesh;
  ICanvas* canvas;
  Scene* scene;
  Camera* mainCamera;
  Mesh* theParent;
  bool local;
  bool snapT;
  bool snapR;
  float transSnap;
  float rotSnap;
  float axesLen;
  float axesScale;
  StandardMaterial* redMat;
  StandardMaterial* greenMat;
  StandardMaterial* blueMat;
  StandardMaterial* whiteMat;
  StandardMaterial* yellowMat;
  ActHist actHist;
  ::std::function<void(Scene* scene, EventState& es)> renderer;
  // EventListener* pointerdown;
  // EventListener* pointerup;
  // EventListener* pointermove;
  // Axes visibility
  float visibility;
  // Picking
  bool pDown;
  Mesh* axisPicked;
  Mesh* prevOverMesh;
  bool pointerIsOver;
  Material* savedMat;
  Color3 savedCol;
  bool editing;
  Vector3 prevPos;
  float snapRX;
  float snapRY;
  float snapRZ;
  Vector3 snapTV;
  Vector3 transBy;
  bool snapS;
  int snapSX;
  int snapSY;
  int snapSZ;
  int snapSA;
  Vector3 snapSV;
  float scaleSnap;
  Vector3 scale;
  bool eulerian;
  float snapRA;
  bool transEnabled;
  bool rotEnabled;
  bool scaleEnabled;
  LinesMesh* bXaxis;
  LinesMesh* bYaxis;
  LinesMesh* bZaxis;
  LinesMesh* xaxis;
  LinesMesh* yaxis;
  LinesMesh* zaxis;
  Mesh* guideCtl;
  Mesh* pickPlanes;
  Mesh* pickPlane;
  Mesh* pALL;
  Mesh* pXZ;
  Mesh* pZY;
  Mesh* pYX;
  Mesh* tCtl;
  Mesh* tX;
  Mesh* tY;
  Mesh* tZ;
  Mesh* tXZ;
  Mesh* tZY;
  Mesh* tYX;
  Mesh* tAll;
  Mesh* tEndX;
  Mesh* tEndY;
  Mesh* tEndZ;
  Mesh* tEndXZ;
  Mesh* tEndZY;
  Mesh* tEndYX;
  Mesh* tEndAll;
  Mesh* rCtl;
  Mesh* rX;
  Mesh* rY;
  Mesh* rZ;
  Mesh* rAll;
  LinesMesh* rEndX;
  LinesMesh* rEndY;
  LinesMesh* rEndZ;
  LinesMesh* rEndAll;
  Mesh* sCtl;
  Mesh* sX;
  Mesh* sY;
  Mesh* sZ;
  Mesh* sXZ;
  Mesh* sZY;
  Mesh* sYX;
  Mesh* sAll;
  Mesh* sEndX;
  Mesh* sEndY;
  Mesh* sEndZ;
  Mesh* sEndXZ;
  Mesh* sEndZY;
  Mesh* sEndYX;
  Mesh* sEndAll;
  Vector3 localX;
  Vector3 localY;
  Vector3 localZ;
  Vector3 tSnap;
  // How far away from camera should the axis appear to be
  float distFromCamera;
  // Vector from camera to axes parent
  Vector3 toParent;
  Vector3 cameraNormal;

}; // end of class EditControl

} // end of namespace Extensions
} // end of namespace BABYLON

#endif // end of BABYLON_EXTENSIONS_EDIT_CONTROL_EDIT_CONTROL_H
