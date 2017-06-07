#ifndef BABYLON_EXTENSIONS_EDIT_CONTROL_EDIT_CONTROL_H
#define BABYLON_EXTENSIONS_EDIT_CONTROL_EDIT_CONTROL_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/extensions/editcontrol/act_history.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {
namespace Extensions {

class EditControl {

public:
  EditControl(Mesh* mesh, Camera* camera, ICanvas* canvas, float scale);
  ~EditControl();

  void switchTo(Mesh* mesh);
  void setUndoCount(int c);
  void undo();
  void redo();
  void detach();
  void disposeAll();
  bool isEditing() const;
  bool isPointerOver();
  bool isTranslationEnabled();
  void enableTranslation();
  void disableTranslation();
  bool isRotationEnabled();
  void returnEuler(bool euler);
  void enableRotation();
  void disableRotation();
  bool isScalingEnabled();
  void enableScaling();
  void disableScaling();
  void setLocal(bool l);
  bool isLocal();
  void setTransSnap(bool s);
  void setRotSnap(bool s);
  void setScaleSnap(bool s);
  void setTransSnapValue(float t);
  void setRotSnapValue(float r);
  void setScaleSnapValue(float r);

private:
  void renderLoopProcess();
  void onPointerDown(const Event& event);
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
  void createGuideAxes();
  void createPickPlanes();
  void createTransAxes();
  void createTriangle(const std::string& name, float w, Scene* scene);
  void createRotAxes();
  Mesh* extrudeBox(float w, float l);
  LinesMesh* createCircle(float r, float t);
  Mesh* createTube();
  void createScaleAxes();
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
  int transSnap;
  float rotSnap;
  float axesLen;
  int axesScale;
  StandardMaterial* redMat;
  StandardMaterial* greenMat;
  StandardMaterial* blueMat;
  StandardMaterial* whiteMat;
  StandardMaterial* yellowMat;
  ActHist actHist;
  std::function<void()> renderer;
  EventListener* pointerdown;
  EventListener* pointerup;
  EventListener* pointermove;
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
  int snapRX;
  int snapRY;
  int snapRZ;
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
  int snapRA;
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
