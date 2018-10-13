#include <babylon/extensions/editcontrol/edit_control.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/materials/standard_material.h>
#include <babylon/math/axis.h>
#include <babylon/math/path2.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/mesh/mesh_builder.h>
#include <babylon/mesh/polygonmesh/polygon_mesh_builder.h>
#include <babylon/mesh/vertex_data_options.h>

namespace BABYLON {
namespace Extensions {

EditControl::EditControl(const MeshPtr& iMesh, Camera* camera, ICanvas* iCanvas,
                         float scale)
    : mesh{iMesh}
    , canvas{iCanvas}
    , scene{mesh->getScene()}
    , mainCamera{camera}
    , theParent{Mesh::New("EditControl", scene)}
    , local{true}
    , snapT{false}
    , snapR{false}
    , transSnap{1}
    , rotSnap{Math::PI / 18.f}
    , axesLen{0.4f}
    , axesScale{scale}
    , redMat{nullptr}
    , greenMat{nullptr}
    , blueMat{nullptr}
    , whiteMat{nullptr}
    , yellowMat{nullptr}
    , actHist{ActHist(mesh, 10)}
    , visibility{0.7f}
    , pDown{false}
    , axisPicked{nullptr}
    , prevOverMesh{nullptr}
    , pointerIsOver{false}
    , savedMat{nullptr}
    , editing{false}
    , snapRX{0}
    , snapRY{0}
    , snapRZ{0}
    , snapTV{Vector3(0.f, 0.f, 0.f)}
    , transBy{Vector3(0.f, 0.f, 0.f)}
    , snapS{false}
    , snapSX{0}
    , snapSY{0}
    , snapSZ{0}
    , snapSA{0}
    , snapSV{Vector3(0.f, 0.f, 0.f)}
    , scaleSnap{0.25f}
    , scale{Vector3(0.f, 0.f, 0.f)}
    , eulerian{false}
    , snapRA{0}
    , transEnabled{false}
    , rotEnabled{false}
    , scaleEnabled{false}
    , bXaxis{nullptr}
    , bYaxis{nullptr}
    , bZaxis{nullptr}
    , xaxis{nullptr}
    , yaxis{nullptr}
    , zaxis{nullptr}
    , guideCtl{nullptr}
    , pickPlanes{nullptr}
    , pickPlane{nullptr}
    , pALL{nullptr}
    , pXZ{nullptr}
    , pZY{nullptr}
    , pYX{nullptr}
    , tCtl{nullptr}
    , tX{nullptr}
    , tY{nullptr}
    , tZ{nullptr}
    , tXZ{nullptr}
    , tZY{nullptr}
    , tYX{nullptr}
    , tAll{nullptr}
    , tEndX{nullptr}
    , tEndY{nullptr}
    , tEndZ{nullptr}
    , tEndXZ{nullptr}
    , tEndZY{nullptr}
    , tEndYX{nullptr}
    , tEndAll{nullptr}
    , rCtl{nullptr}
    , rX{nullptr}
    , rY{nullptr}
    , rZ{nullptr}
    , rAll{nullptr}
    , rEndX{nullptr}
    , rEndY{nullptr}
    , rEndZ{nullptr}
    , rEndAll{nullptr}
    , sCtl{nullptr}
    , sX{nullptr}
    , sY{nullptr}
    , sZ{nullptr}
    , sXZ{nullptr}
    , sZY{nullptr}
    , sYX{nullptr}
    , sAll{nullptr}
    , sEndX{nullptr}
    , sEndY{nullptr}
    , sEndZ{nullptr}
    , sEndXZ{nullptr}
    , sEndZY{nullptr}
    , sEndYX{nullptr}
    , sEndAll{nullptr}
    , localX{Vector3(0.f, 0.f, 0.f)}
    , localY{Vector3(0.f, 0.f, 0.f)}
    , localZ{Vector3(0.f, 0.f, 0.f)}
    , distFromCamera{2}
    , toParent{Vector3(0.f, 0.f, 0.f)}
    , cameraNormal{Vector3(0.f, 0.f, 0.f)}
{
  mesh->computeWorldMatrix(true);
  theParent = Mesh::New("EditControl", scene);
  mesh->getAbsolutePivotPointToRef(theParent->position());
  theParent->rotationQuaternion = mesh->rotationQuaternion();
  theParent->visibility         = 0.f;
  theParent->isPickable         = false;
  createMaterials(scene);
  createGuideAxes();
  guideCtl->setParent(theParent.get());
  createPickPlanes();
  pickPlanes->setParent(theParent.get());
  // pointerdown = (evt) => { return onPointerDown(evt) };
  // pointerup = (evt) => { return onPointerUp(evt) };
  // pointermove = (evt) => { return onPointerMove(evt) };
  // canvas->addEventListener("pointerdown", pointerdown, false);
  // canvas->addEventListener("pointerup", pointerup, false);
  // canvas->addEventListener("pointermove", pointermove, false);
  setLocalAxes(mesh);
  renderer = [this](Scene*, EventState&) { renderLoopProcess(); };
  scene->registerBeforeRender(renderer);
}

EditControl::~EditControl()
{
}

void EditControl::renderLoopProcess()
{
  setAxesScale();
  mesh->getAbsolutePivotPointToRef(theParent->position());
  onPointerOver();
}

void EditControl::switchTo(const MeshPtr& iMesh)
{
  iMesh->computeWorldMatrix(true);
  mesh                          = iMesh;
  theParent->rotationQuaternion = iMesh->rotationQuaternion();
  setLocalAxes(iMesh);
  actHist = ActHist(iMesh, 10);
}

void EditControl::setUndoCount(int c)
{
  actHist.setCapacity(c);
}

void EditControl::undo()
{
  actHist.undo();
  mesh->computeWorldMatrix(true);
  setLocalAxes(mesh);
}

void EditControl::redo()
{
  actHist.redo();
  mesh->computeWorldMatrix(true);
  setLocalAxes(mesh);
}

void EditControl::detach()
{
  // canvas->removeEventListener("pointerdown", pointerdown, false);
  // canvas.removeEventListener("pointerup", pointerup, false);
  // canvas.removeEventListener("pointermove", pointermove, false);
  scene->unregisterBeforeRender(renderer);
  disposeAll();
}

void EditControl::disposeAll()
{
  theParent->dispose();
  disposeMaterials();
}

void EditControl::onPointerDown(PointerEvent& evt)
{
  evt.preventDefault();
  pDown = true;
  if (evt.button != MouseButtonType::LEFT) {
    return;
  }

  auto pickResult = scene->pick(
    scene->pointerX(), scene->pointerY(),
    [this](const AbstractMeshPtr& mesh) {
      if (transEnabled) {
        if ((mesh == tX) || (mesh == tY) || (mesh == tZ) || (mesh == tXZ)
            || (mesh == tZY) || (mesh == tYX) || (mesh == tAll)) {
          return true;
        }
      }
      else if ((rotEnabled)) {
        if ((mesh == rX) || (mesh == rY) || (mesh == rZ) || (mesh == rAll)) {
          return true;
        }
      }
      else if ((scaleEnabled)) {
        if ((mesh == sX) || (mesh == sY) || (mesh == sZ) || (mesh == sXZ)
            || (mesh == sZY) || (mesh == sYX) || (mesh == sAll)) {
          return true;
        }
      }
      return false;
    },
    false, mainCamera);

  if ((*pickResult).hit) {
    axisPicked  = static_cast<Mesh*>((*pickResult).pickedMesh);
    auto childs = axisPicked->getChildren();
    if (!childs.empty()) {
      std::static_pointer_cast<Mesh>(childs[0])->visibility = visibility;
    }
    else {
      axisPicked->visibility = visibility;
    }
    const auto& name = axisPicked->name;
    if (name == "X") {
      bXaxis->visibility = 1.f;
    }
    else if (name == "Y") {
      bYaxis->visibility = 1.f;
    }
    else if (name == "Z") {
      bZaxis->visibility = 1.f;
    }
    else if (name == "XZ") {
      bXaxis->visibility = 1.f;
      bZaxis->visibility = 1.f;
    }
    else if ((name == "ZY")) {
      bZaxis->visibility = 1.f;
      bYaxis->visibility = 1.f;
    }
    else if ((name == "YX")) {
      bYaxis->visibility = 1.f;
      bXaxis->visibility = 1.f;
    }
    else if ((name == "ALL")) {
      bXaxis->visibility = 1.f;
      bYaxis->visibility = 1.f;
      bZaxis->visibility = 1.f;
    }
    editing = true;
    // lets find out where we are on the pickplane
    pickPlane = getPickPlane(axisPicked);
    Vector3 _prevPos;
    bool hasPos;
    std::tie(_prevPos, hasPos) = getPosOnPickPlane();
    if (hasPos) {
      prevPos = _prevPos;
    }
    detachControl(mainCamera.get(), canvas);
  }
}

bool EditControl::isEditing() const
{
  return editing;
}

void EditControl::detachControl(Camera* cam, ICanvas* can)
{
  cam->detachControl(can);
}

bool EditControl::isPointerOver() const
{
  return pointerIsOver;
}

void EditControl::onPointerOver()
{
  if (pDown) {
    return;
  }

  auto pickResult = scene->pick(
    scene->pointerX(), scene->pointerY(),
    [this](const AbstractMeshPtr& mesh) {
      if (transEnabled) {
        if ((mesh == tX) || (mesh == tY) || (mesh == tZ) || (mesh == tXZ)
            || (mesh == tZY) || (mesh == tYX) || (mesh == tAll)) {
          return true;
        }
      }
      else if ((rotEnabled)) {
        if ((mesh == rX) || (mesh == rY) || (mesh == rZ) || (mesh == rAll)) {
          return true;
        }
      }
      else if (scaleEnabled) {
        if ((mesh == sX) || (mesh == sY) || (mesh == sZ) || (mesh == sXZ)
            || (mesh == sZY) || (mesh == sYX) || (mesh == sAll)) {
          return true;
        }
      }
      return false;
    },
    false, mainCamera);

  if ((*pickResult).hit) {
    if (static_cast<Mesh*>((*pickResult).pickedMesh) != prevOverMesh) {
      pointerIsOver = true;
      if (prevOverMesh != nullptr) {
        prevOverMesh->visibility = 0.f;
        restoreColor(prevOverMesh);
      }
      prevOverMesh = static_cast<Mesh*>((*pickResult).pickedMesh);
      if (rotEnabled) {
        savedCol
          = (std::static_pointer_cast<LinesMesh>(prevOverMesh->getChildren()[0])
               ->color);
        std::static_pointer_cast<LinesMesh>(prevOverMesh->getChildren()[0])
          ->color
          = Color3::White();
      }
      else {
        auto childs = prevOverMesh->getChildren();
        if (!childs.empty()) {
          savedMat = std::static_pointer_cast<Mesh>(childs[0])->material();
          std::static_pointer_cast<Mesh>(childs[0])->material = whiteMat;
        }
        else {
          savedMat               = prevOverMesh->material();
          prevOverMesh->material = whiteMat;
        }
      }
      if (prevOverMesh->name == "X") {
        xaxis->color = Color3::White();
      }
      else if (prevOverMesh->name == "Y") {
        yaxis->color = Color3::White();
      }
      else if (prevOverMesh->name == "Z") {
        zaxis->color = Color3::White();
      }
    }
  }
  else {
    pointerIsOver = false;
    if (prevOverMesh != nullptr) {
      restoreColor(prevOverMesh);
      prevOverMesh = nullptr;
    }
  }
}

void EditControl::restoreColor(Mesh* mesh)
{
  const auto& meshName = mesh->name;
  if (meshName == "X") {
    xaxis->color = Color3::Red();
  }
  else if (meshName == "Y") {
    yaxis->color = Color3::Green();
  }
  else if (meshName == "Z") {
    zaxis->color = Color3::Blue();
  }

  if (rotEnabled) {
    std::static_pointer_cast<LinesMesh>(mesh->getChildren()[0])->color
      = savedCol;
  }
  else {
    auto childs = mesh->getChildren();
    if (!childs.empty()) {
      std::static_pointer_cast<Mesh>(childs[0])->material = savedMat;
    }
    else {
      mesh->material = savedMat;
    }
  }
}

void EditControl::onPointerUp(const Event& /*evt*/)
{
  pDown = false;
  if (editing) {
    mainCamera->attachControl(canvas);
    editing = false;
    hideBaxis();
    restoreColor(prevOverMesh);
    prevOverMesh = nullptr;
    actHist.add();
  }
}

void EditControl::onPointerMove(const Event& /*evt*/)
{
  if (!pDown || !editing) {
    return;
  }

  pickPlane = getPickPlane(axisPicked);

  Vector3 newPos;
  bool hasPos;
  std::tie(newPos, hasPos) = getPosOnPickPlane();
  if (!hasPos) {
    return;
  }

  auto diff = newPos.subtract(prevPos);
  if (stl_util::almost_equal(diff.x, 0.f) && stl_util::almost_equal(diff.y, 0.f)
      && stl_util::almost_equal(diff.z, 0.f)) {
    return;
  }

  if (transEnabled) {
    doTranslation(diff);
  }
  if (scaleEnabled && local) {
    doScaling(diff);
  }
  if (rotEnabled) {
    doRotation(mesh, axisPicked, newPos);
  }

  prevPos = newPos;
}

MeshPtr EditControl::getPickPlane(Mesh* axis)
{
  const auto& n = axis->name;
  if (transEnabled || scaleEnabled) {
    if (n == "XZ") {
      return pXZ;
    }
    else if (n == "ZY") {
      return pZY;
    }
    else if (n == "YX") {
      return pYX;
    }
    else if (n == "ALL") {
      return pALL;
    }
    else {
      // Get the position of camera in the mesh frame of reference
      const auto invMat = mesh->getWorldMatrix()->copy().invert();
      const auto c
        = Vector3::TransformCoordinates(mainCamera->position, invMat);
      const auto s = mesh->scaling();
      if (n == "X") {
        if (std::abs(c.y * s.y) > std::abs(c.z * s.z)) {
          return pXZ;
        }
        else {
          return pYX;
        }
      }
      else if (n == "Z") {
        if (std::abs(c.y * s.y) > std::abs(c.x * s.x)) {
          return pXZ;
        }
        else {
          return pZY;
        }
      }
      else if (n == "Y") {
        if (std::abs(c.z * s.z) > std::abs(c.x * s.x)) {
          return pYX;
        }
        else {
          return pZY;
        }
      }
    }
  }
  else if (rotEnabled) {
    if (n == "X") {
      return pZY;
    }
    else if (n == "Y") {
      return pXZ;
    }
    else if (n == "Z") {
      return pYX;
    }
    else {
      return pALL;
    }
  }
  else {
    return nullptr;
  }

  return nullptr;
}

void EditControl::doTranslation(const Vector3& diff)
{
  transBy.x     = 0;
  transBy.y     = 0;
  transBy.z     = 0;
  const auto& n = axisPicked->name;
  if ((n == "X") || (n == "XZ") || (n == "YX") || (n == "ALL")) {
    if (local) {
      transBy.x
        = Vector3::Dot(diff, localX) / (localX.length() * mesh->scaling().x);
    }
    else {
      transBy.x = diff.x;
    }
  }
  if ((n == "Y") || (n == "ZY") || (n == "YX") || (n == "ALL")) {
    if (local) {
      transBy.y
        = Vector3::Dot(diff, localY) / (localY.length() * mesh->scaling().y);
    }
    else {
      transBy.y = diff.y;
    }
  }
  if ((n == "Z") || (n == "XZ") || (n == "ZY") || (n == "ALL")) {
    if (local) {
      transBy.z
        = Vector3::Dot(diff, localZ) / (localZ.length() * mesh->scaling().z);
    }
    else {
      transBy.z = diff.z;
    }
  }
  transWithSnap(mesh, transBy, local);
  mesh->computeWorldMatrix(true);
}

void EditControl::transWithSnap(const MeshPtr& mesh, Vector3& trans, bool local)
{
  if (snapT) {
    bool snapit = false;
    snapTV.addInPlace(trans);
    if (std::abs(snapTV.x) > (tSnap.x / mesh->scaling().x)) {
      if (snapTV.x > 0) {
        trans.x = tSnap.x;
      }
      else {
        trans.x = -tSnap.x;
      }
      trans.x = trans.x / mesh->scaling().x;
      snapit  = true;
    }
    if (std::abs(snapTV.y) > (tSnap.y / mesh->scaling().y)) {
      if (snapTV.y > 0) {
        trans.y = tSnap.y;
      }
      else {
        trans.y = -tSnap.y;
      }
      trans.y = trans.y / mesh->scaling().y;
      snapit  = true;
    }
    if (std::abs(snapTV.z) > (tSnap.z / mesh->scaling().z)) {
      if (snapTV.z > 0) {
        trans.z = tSnap.z;
      }
      else {
        trans.z = -tSnap.z;
      }
      trans.z = trans.z / mesh->scaling().z;
      snapit  = true;
    }
    if (!snapit) {
      return;
    }
    if (!stl_util::almost_equal(std::abs(trans.x),
                                tSnap.x / mesh->scaling().x)) {
      trans.x = 0.f;
    }
    if (!stl_util::almost_equal(std::abs(trans.y),
                                tSnap.y / mesh->scaling().y)) {
      trans.y = 0.f;
    }
    if (!stl_util::almost_equal(std::abs(trans.z),
                                tSnap.z / mesh->scaling().z)) {
      trans.z = 0.f;
    }
    Vector3::FromFloatsToRef(0, 0, 0, snapTV);
    snapit = false;
  }

  if (local) {
    mesh->locallyTranslate(trans);
  }
  else {
    mesh->position().addInPlace(trans);
  }
}

void EditControl::doScaling(const Vector3& diff)
{
  scale.x       = 0.f;
  scale.y       = 0.f;
  scale.z       = 0.f;
  const auto& n = axisPicked->name;
  if ((n == "X") || (n == "XZ") || (n == "YX") || (n == "ALL")) {
    scale.x = Vector3::Dot(diff, localX) / localX.length();
  }
  if ((n == "Y") || (n == "ZY") || (n == "YX") || (n == "ALL")) {
    scale.y = Vector3::Dot(diff, localY) / localY.length();
  }
  if ((n == "Z") || (n == "XZ") || (n == "ZY") || (n == "ALL")) {
    scale.z = Vector3::Dot(diff, localZ) / localZ.length();
  }
  if (n == "ALL") {
    scale.copyFromFloats(scale.y, scale.y, scale.y);
  }
  scaleWithSnap(mesh, scale);
}

void EditControl::scaleWithSnap(const MeshPtr& mesh, Vector3& p)
{
  if (snapS) {
    bool snapit = false;
    snapSV.addInPlace(p);
    if (std::abs(snapSV.x) > scaleSnap) {
      if (p.x > 0) {
        p.x = scaleSnap;
      }
      else {
        p.x = -scaleSnap;
      }
      snapit = true;
    }
    if (std::abs(snapSV.y) > scaleSnap) {
      if (p.y > 0) {
        p.y = scaleSnap;
      }
      else {
        p.y = -scaleSnap;
      }
      snapit = true;
    }
    if (std::abs(snapSV.z) > scaleSnap) {
      if (p.z > 0) {
        p.z = scaleSnap;
      }
      else {
        p.z = -scaleSnap;
      }
      snapit = true;
    }
    if (!snapit) {
      return;
    }
    if (!stl_util::almost_equal(std::abs(p.x), scaleSnap)
        && !stl_util::almost_equal(p.x, 0.f)) {
      p.x = 0;
    }
    if (!stl_util::almost_equal(std::abs(p.y), scaleSnap)
        && !stl_util::almost_equal(p.y, 0.f)) {
      p.y = 0;
    }
    if (!stl_util::almost_equal(std::abs(p.z), scaleSnap)
        && !stl_util::almost_equal(p.z, 0.f)) {
      p.z = 0;
    }
    Vector3::FromFloatsToRef(0.f, 0.f, 0.f, snapSV);
    snapit = false;
  }
  mesh->scaling().addInPlace(p);
}

void EditControl::doRotation(const MeshPtr& mesh, Mesh* axis,
                             const Vector3& newPos)
{
  auto cN = Vector3::TransformNormal(Axis::Z(), *mainCamera->getWorldMatrix());
  auto angle
    = EditControl::getAngle(prevPos, newPos, mesh->getAbsolutePivotPoint(), cN);

  if (axis == rX.get()) {
    if (snapR) {
      snapRX += angle;
      angle = 0.f;
      if (std::abs(snapRX) >= rotSnap) {
        if ((snapRX > 0)) {
          angle = rotSnap;
        }
        else {
          angle = -rotSnap;
        }
        snapRX = 0;
      }
    }
    if (!stl_util::almost_equal(angle, 0.f)) {
      if (local) {
        if (Vector3::Dot(localX, cN) < 0) {
          angle = -1.f * angle;
        }
        auto axis = Axis::X();
        mesh->rotate(axis, angle, Space::LOCAL);
      }
      else {
        auto axis = Vector3(cN.x, 0, 0);
        mesh->rotate(axis, angle, Space::WORLD);
      }
    }
  }
  else if (axis == rY.get()) {
    if (snapR) {
      snapRY += angle;
      angle = 0.f;
      if (std::abs(snapRY) >= rotSnap) {
        if ((snapRY > 0)) {
          angle = rotSnap;
        }
        else {
          angle = -rotSnap;
        }
        snapRY = 0;
      }
    }
    if (!stl_util::almost_equal(angle, 0.f)) {
      if (local) {
        if (Vector3::Dot(localY, cN) < 0) {
          angle = -1.f * angle;
        }
        auto axis = Axis::Y();
        mesh->rotate(axis, angle, Space::LOCAL);
      }
      else {
        auto axis = Vector3(0.f, cN.y, 0.f);
        mesh->rotate(axis, angle, Space::WORLD);
      }
    }
  }
  else if (axis == rZ.get()) {
    if (snapR) {
      snapRZ += angle;
      angle = 0.f;
      if (std::abs(snapRZ) >= rotSnap) {
        if (snapRZ > 0)
          angle = rotSnap;
        else
          angle = -rotSnap;
        snapRZ = 0;
      }
    }
    if (!stl_util::almost_equal(angle, 0.f)) {
      if (local) {
        if (Vector3::Dot(localZ, cN) < 0) {
          angle = -1.f * angle;
        }
        auto axis = Axis::Z();
        mesh->rotate(axis, angle, Space::LOCAL);
      }
      else {
        auto axis = Vector3(0.f, 0.f, cN.z);
        mesh->rotate(axis, angle, Space::WORLD);
      }
    }
  }
  else if (axis == rAll.get()) {
    if (snapR) {
      snapRA += angle;
      angle = 0.f;
      if (std::abs(snapRA) >= rotSnap) {
        if (snapRA > 0) {
          angle = rotSnap;
        }
        else {
          angle = -rotSnap;
        }
        snapRA = 0;
      }
    }
    if (!stl_util::almost_equal(angle, 0.f)) {
      auto axis = mesh->position().subtract(mainCamera->position);
      mesh->rotate(axis, angle, Space::WORLD);
    }
    setLocalAxes(mesh);
    if ((eulerian)) {
      mesh->rotation           = mesh->rotationQuaternion()->toEulerAngles();
      mesh->rotationQuaternion = std::nullopt;
    }
  }
}

std::tuple<Vector3, bool> EditControl::getPosOnPickPlane()
{
  auto pickinfo = scene->pick(
    scene->pointerX(), scene->pointerY(),
    [this](const AbstractMeshPtr& mesh) { return mesh == pickPlane; }, false,
    mainCamera);
  if ((*pickinfo).hit) {
    return std::make_tuple(*(*pickinfo).pickedPoint, true);
  }
  else {
    return std::make_tuple(Vector3::Zero(), false);
  }
}

void EditControl::hideBaxis()
{
  bXaxis->visibility = 0.f;
  bYaxis->visibility = 0.f;
  bZaxis->visibility = 0.f;
}

bool EditControl::isTranslationEnabled() const
{
  return transEnabled;
}

void EditControl::enableTranslation()
{
  if (tX == nullptr) {
    createTransAxes();
    tCtl->setParent(theParent.get());
  }
  if (!transEnabled) {
    tEndX->visibility   = visibility;
    tEndY->visibility   = visibility;
    tEndZ->visibility   = visibility;
    tEndXZ->visibility  = visibility;
    tEndZY->visibility  = visibility;
    tEndYX->visibility  = visibility;
    tEndAll->visibility = visibility;
    transEnabled        = true;
    disableRotation();
    disableScaling();
  }
}

void EditControl::disableTranslation()
{
  if (transEnabled) {
    tEndX->visibility   = 0.f;
    tEndY->visibility   = 0.f;
    tEndZ->visibility   = 0.f;
    tEndXZ->visibility  = 0.f;
    tEndZY->visibility  = 0.f;
    tEndYX->visibility  = 0.f;
    tEndAll->visibility = 0.f;
    transEnabled        = false;
  }
}

bool EditControl::isRotationEnabled() const
{
  return rotEnabled;
}

void EditControl::returnEuler(bool euler)
{
  eulerian = euler;
}

void EditControl::enableRotation()
{
  if (rX == nullptr) {
    createRotAxes();
    rCtl->setParent(theParent.get());
  }
  if (!rotEnabled) {
    rEndX->visibility   = visibility;
    rEndY->visibility   = visibility;
    rEndZ->visibility   = visibility;
    rEndAll->visibility = visibility;
    rotEnabled          = true;
    disableTranslation();
    disableScaling();
  }
}

void EditControl::disableRotation()
{
  if (rotEnabled) {
    rEndX->visibility   = 0.f;
    rEndY->visibility   = 0.f;
    rEndZ->visibility   = 0.f;
    rEndAll->visibility = 0.f;
    rotEnabled          = false;
  }
}

bool EditControl::isScalingEnabled() const
{
  return scaleEnabled;
}

void EditControl::enableScaling()
{
  if (sX == nullptr) {
    createScaleAxes();
    sCtl->setParent(theParent.get());
  }
  if (!scaleEnabled) {
    sEndX->visibility   = visibility;
    sEndY->visibility   = visibility;
    sEndZ->visibility   = visibility;
    sEndXZ->visibility  = visibility;
    sEndZY->visibility  = visibility;
    sEndYX->visibility  = visibility;
    sEndAll->visibility = visibility;
    scaleEnabled        = true;
    disableTranslation();
    disableRotation();
  }
}

void EditControl::disableScaling()
{
  if (scaleEnabled) {
    sEndX->visibility   = 0.f;
    sEndY->visibility   = 0.f;
    sEndZ->visibility   = 0.f;
    sEndXZ->visibility  = 0.f;
    sEndZY->visibility  = 0.f;
    sEndYX->visibility  = 0.f;
    sEndAll->visibility = 0.f;
    scaleEnabled        = false;
  }
}

void EditControl::createGuideAxes()
{
  guideCtl = Mesh::New("guideCtl", scene);

  // the big axes, shown when an axis is selected
  bXaxis = Mesh::CreateLines(
    "bxAxis", {Vector3(-100.f, 0.f, 0.f), Vector3(100.f, 0.f, 0.f)}, scene);
  bYaxis = Mesh::CreateLines(
    "byAxis", {Vector3(0.f, -100.f, 0.f), Vector3(0.f, 100.f, 0.f)}, scene);
  bZaxis = Mesh::CreateLines(
    "bzAxis", {Vector3(0.f, 0.f, -100.f), Vector3(0.f, 0.f, 100.f)}, scene);

  // lines are now pickable too
  bXaxis->isPickable = false;
  bYaxis->isPickable = false;
  bZaxis->isPickable = false;

  bXaxis->setParent(guideCtl.get());
  bYaxis->setParent(guideCtl.get());
  bZaxis->setParent(guideCtl.get());
  bXaxis->color = Color3::Red();
  bYaxis->color = Color3::Green();
  bZaxis->color = Color3::Blue();
  hideBaxis();

  // the small axis
  float al = axesLen * axesScale;
  xaxis    = Mesh::CreateLines(
    "xAxis", {Vector3(0.f, 0.f, 0.f), Vector3(al, 0.f, 0.f)}, scene);
  yaxis = Mesh::CreateLines(
    "yAxis", {Vector3(0.f, 0.f, 0.f), Vector3(0.f, al, 0.f)}, scene);
  zaxis = Mesh::CreateLines(
    "zAxis", {Vector3(0.f, 0.f, 0.f), Vector3(0.f, 0.f, al)}, scene);

  // lines are now pickable too
  xaxis->isPickable = false;
  yaxis->isPickable = false;
  zaxis->isPickable = false;

  xaxis->setParent(guideCtl.get());
  yaxis->setParent(guideCtl.get());
  zaxis->setParent(guideCtl.get());
  xaxis->color            = Color3::Red();
  yaxis->color            = Color3::Green();
  zaxis->color            = Color3::Blue();
  xaxis->renderingGroupId = 2;
  yaxis->renderingGroupId = 2;
  zaxis->renderingGroupId = 2;
}

void EditControl::createPickPlanes()
{
  pALL = Mesh::CreatePlane("pALL", 5, scene);
  pXZ  = Mesh::CreatePlane("pXZ", 5, scene);
  pZY  = Mesh::CreatePlane("pZY", 5, scene);
  pYX  = Mesh::CreatePlane("pYX", 5, scene);

  pALL->isPickable = false;
  pXZ->isPickable  = false;
  pZY->isPickable  = false;
  pYX->isPickable  = false;

  pALL->visibility = 0.f;
  pXZ->visibility  = 0.f;
  pZY->visibility  = 0.f;
  pYX->visibility  = 0.f;

  pALL->renderingGroupId = 1;
  pXZ->renderingGroupId  = 1;
  pZY->renderingGroupId  = 1;
  pYX->renderingGroupId  = 1;

  pALL->billboardMode = Mesh::BILLBOARDMODE_ALL;
  auto axis           = Axis::X();
  pXZ->rotate(axis, 1.57f);
  axis = Axis::Y();
  pZY->rotate(axis, 1.57f);

  pickPlanes = Mesh::New("pickPlanes", scene);
  pALL->setParent(theParent.get());
  pXZ->setParent(pickPlanes.get());
  pZY->setParent(pickPlanes.get());
  pYX->setParent(pickPlanes.get());
}

void EditControl::createTransAxes()
{
  const float r = 0.04f * axesScale;

  const float l = axesLen * axesScale;
  tCtl          = Mesh::New("tarnsCtl", scene);

  // pickable invisible boxes around axes lines
  tX       = extrudeBox(r / 2.f, l);
  tX->name = "X";
  tY       = tX->clone("Y");
  tZ       = tX->clone("Z");

  PlaneOptions planeOptions(r * 2.f);
  tXZ = MeshBuilder::CreatePlane("XZ", planeOptions, scene);
  tZY = tXZ->clone("ZY");
  tYX = tXZ->clone("YX");

  tXZ->rotation().x = 1.57f;
  tZY->rotation().y = -1.57f;

  tXZ->position().x = r;
  tXZ->position().z = r;

  tZY->position().z = r;
  tZY->position().y = r;

  tYX->position().y = r;
  tYX->position().x = r;

  tAll = Mesh::CreateBox("ALL", r * 2.f, scene);

  tX->setParent(tCtl.get());
  tY->setParent(tCtl.get());
  tZ->setParent(tCtl.get());
  tXZ->setParent(tCtl.get());
  tZY->setParent(tCtl.get());
  tYX->setParent(tCtl.get());
  tAll->setParent(tCtl.get());

  tX->rotation().y = 1.57f;
  tY->rotation().x -= 1.57f;

  tX->visibility   = 0.f;
  tY->visibility   = 0.f;
  tZ->visibility   = 0.f;
  tXZ->visibility  = 0.f;
  tZY->visibility  = 0.f;
  tYX->visibility  = 0.f;
  tAll->visibility = 0.f;

  tX->renderingGroupId   = 1;
  tY->renderingGroupId   = 1;
  tZ->renderingGroupId   = 1;
  tXZ->renderingGroupId  = 1;
  tZY->renderingGroupId  = 1;
  tYX->renderingGroupId  = 1;
  tAll->renderingGroupId = 1;

  // Do not want clients picking this
  // we will pick using mesh filter in scene.pick function
  tX->isPickable   = false;
  tY->isPickable   = false;
  tZ->isPickable   = false;
  tXZ->isPickable  = false;
  tZY->isPickable  = false;
  tYX->isPickable  = false;
  tAll->isPickable = false;

  // Non pickable but visible cones at end of axes lines
  // cyl len
  const float cl = l / 5.f;
  // cyl radius
  const float cr = r;
  tEndX          = Mesh::CreateCylinder("tEndX", cl, 0, cr, 6, 1, scene);
  tEndY          = tEndX->clone("tEndY");
  tEndZ          = tEndX->clone("tEndZ");
  tEndXZ         = createTriangle("XZ", cr * 1.75f, scene);
  tEndZY         = tEndXZ->clone("ZY");
  tEndYX         = tEndXZ->clone("YX");
  PolyhedronOptions polyhedronOptions(cr / 2.f);
  polyhedronOptions.type = 1;
  tEndAll = MeshBuilder::CreatePolyhedron("tEndAll", polyhedronOptions, scene);

  tEndX->rotation().x  = 1.57f;
  tEndY->rotation().x  = 1.57f;
  tEndZ->rotation().x  = 1.57f;
  tEndXZ->rotation().x = -1.57f;
  tEndZY->rotation().x = -1.57f;
  tEndYX->rotation().x = -1.57f;

  tEndX->setParent(tX.get());
  tEndY->setParent(tY.get());
  tEndZ->setParent(tZ.get());
  tEndXZ->setParent(tXZ.get());
  tEndZY->setParent(tZY.get());
  tEndYX->setParent(tYX.get());
  tEndAll->setParent(tAll.get());

  tEndX->position().z = l - cl / 2;
  tEndY->position().z = l - cl / 2;
  tEndZ->position().z = l - cl / 2;

  tEndX->material   = redMat;
  tEndY->material   = greenMat;
  tEndZ->material   = blueMat;
  tEndXZ->material  = redMat;
  tEndZY->material  = blueMat;
  tEndYX->material  = greenMat;
  tEndAll->material = yellowMat;

  tEndX->renderingGroupId   = 1;
  tEndY->renderingGroupId   = 1;
  tEndZ->renderingGroupId   = 1;
  tEndXZ->renderingGroupId  = 1;
  tEndZY->renderingGroupId  = 1;
  tEndYX->renderingGroupId  = 1;
  tEndAll->renderingGroupId = 1;

  tEndX->isPickable   = false;
  tEndY->isPickable   = false;
  tEndZ->isPickable   = false;
  tEndXZ->isPickable  = false;
  tEndZY->isPickable  = false;
  tEndYX->isPickable  = false;
  tEndAll->isPickable = false;
}

MeshPtr EditControl::createTriangle(const std::string& name, float w,
                                    Scene* scene) const
{
  auto p = Path2(w / 2.f, -w / 2.f)
             .addLineTo(w / 2.f, w / 2.f)
             .addLineTo(-w / 2.f, w / 2.f)
             .addLineTo(w / 2.f, -w / 2.f);
  PolygonMeshBuilder s(name, p, scene);
  auto t = s.build();
  return t;
}

void EditControl::createRotAxes()
{
  const float d = axesLen * axesScale * 2.f;
  rCtl          = Mesh::New("rotCtl", scene);
  // Pickable invisible torus around the rotation circles
  rX         = createTube(d / 2.f, 90);
  rX->name   = "X";
  rY         = rX->clone("Y");
  rZ         = rX->clone("Z");
  rAll       = createTube(d / 1.75f, 360);
  rAll->name = "ALL";

  rX->setParent(rCtl.get());
  rY->setParent(rCtl.get());
  rZ->setParent(rCtl.get());
  rAll->setParent(pALL.get());

  rX->rotation().z   = 1.57f;
  rZ->rotation().x   = -1.57f;
  rAll->rotation().x = 1.57f;
  rX->visibility     = 0.f;
  rY->visibility     = 0.f;
  rZ->visibility     = 0.f;
  rAll->visibility   = 0.f;

  rX->renderingGroupId   = 1;
  rY->renderingGroupId   = 1;
  rZ->renderingGroupId   = 1;
  rAll->renderingGroupId = 1;

  // Do not want clients picking this
  // we will pick using mesh filter in scene.pick function
  rX->isPickable   = false;
  rY->isPickable   = false;
  rZ->isPickable   = false;
  rAll->isPickable = false;

  // Non pickable but visible circles
  const float cl = d;
  rEndX          = createCircle(cl / 2.f, 90);
  rEndY          = rEndX->clone("");
  rEndZ          = rEndX->clone("");
  rEndAll        = createCircle(cl / 1.75f, 360);

  rEndX->setParent(rX.get());
  rEndY->setParent(rY.get());
  rEndZ->setParent(rZ.get());
  rEndAll->setParent(rAll.get());

  rEndX->color   = Color3::Red();
  rEndY->color   = Color3::Green();
  rEndZ->color   = Color3::Blue();
  rEndAll->color = Color3::Yellow();

  rEndX->renderingGroupId   = 1;
  rEndY->renderingGroupId   = 1;
  rEndZ->renderingGroupId   = 1;
  rEndAll->renderingGroupId = 1;

  rEndX->isPickable   = false;
  rEndY->isPickable   = false;
  rEndZ->isPickable   = false;
  rEndAll->isPickable = false;
}

MeshPtr EditControl::extrudeBox(float w, float l) const
{
  const std::vector<Vector3> shape
    = {Vector3(w, w, 0), Vector3(-w, w, 0), Vector3(-w, -w, 0),
       Vector3(w, -w, 0), Vector3(w, w, 0)};
  const std::vector<Vector3> path = {Vector3(0, 0, 0), Vector3(0, 0, l)};
  auto box = Mesh::ExtrudeShape("", shape, path, 1, 0, 2, scene);
  return box;
}

LinesMeshPtr EditControl::createCircle(float r, int t) const
{
  std::vector<Vector3> points;
  float x;
  float z;
  float a = 3.14f / 180.f;
  for (int i = 0; i <= t; i = i + 10) {
    x = r * std::cos(i * a);
    if (i == 90) {
      z = r;
    }
    else if (i == 270) {
      z = -r;
    }
    else {
      z = r * std::sin(i * a);
    }
    points.emplace_back(Vector3(x, 0, z));
  }
  auto circle = Mesh::CreateLines("", points, scene);
  return circle;
}

MeshPtr EditControl::createTube(float r, int t) const
{
  std::vector<Vector3> points;
  float x;
  float z;
  float a = 3.14f / 180.f;
  for (int i = 0; i <= t; i = i + 30) {
    x = r * std::cos(i * a);
    if (i == 90) {
      z = r;
    }
    else if (i == 270) {
      z = -r;
    }
    else {
      z = r * std::sin(i * a);
    }
    points.emplace_back(Vector3(x, 0, z));
  }
  auto tube
    = Mesh::CreateTube("", points, 0.02f, 3, nullptr, Mesh::NO_CAP(), scene);
  return tube;
}

void EditControl::createScaleAxes()
{
  float r = 0.04f * axesScale;
  float l = axesLen * axesScale;
  sCtl    = Mesh::New("sCtl", scene);

  sX       = extrudeBox(r / 2.f, l);
  sX->name = "X";
  sY       = sX->clone("Y");
  sZ       = sX->clone("Z");

  PlaneOptions planeOptions(r * 2);
  sXZ = MeshBuilder::CreatePlane("XZ", planeOptions, scene);
  sZY = sXZ->clone("ZY");
  sYX = sXZ->clone("YX");

  sXZ->rotation().x = 1.57f;
  sZY->rotation().y = -1.57f;

  sXZ->position().x = r;
  sXZ->position().z = r;

  sZY->position().z = r;
  sZY->position().y = r;

  sYX->position().y = r;
  sYX->position().x = r;

  sAll = Mesh::CreateBox("ALL", r * 2, scene);

  sX->material   = redMat;
  sY->material   = greenMat;
  sZ->material   = blueMat;
  sAll->material = yellowMat;

  sX->setParent(sCtl.get());
  sY->setParent(sCtl.get());
  sZ->setParent(sCtl.get());
  sAll->setParent(sCtl.get());
  sXZ->setParent(sCtl.get());
  sZY->setParent(sCtl.get());
  sYX->setParent(sCtl.get());

  sX->rotation().y = 1.57f;
  sY->rotation().x -= 1.57f;
  sX->visibility   = 0.f;
  sY->visibility   = 0.f;
  sZ->visibility   = 0.f;
  sXZ->visibility  = 0.f;
  sZY->visibility  = 0.f;
  sYX->visibility  = 0.f;
  sAll->visibility = 0.f;

  sX->renderingGroupId   = 1;
  sY->renderingGroupId   = 1;
  sZ->renderingGroupId   = 1;
  sXZ->renderingGroupId  = 1;
  sZY->renderingGroupId  = 1;
  sYX->renderingGroupId  = 1;
  sAll->renderingGroupId = 1;

  // Do not want clients picking this
  // we will pick using mesh filter in scene.pick function
  sX->isPickable   = false;
  sY->isPickable   = false;
  sZ->isPickable   = false;
  sXZ->isPickable  = false;
  sZY->isPickable  = false;
  sYX->isPickable  = false;
  sAll->isPickable = false;

  // Mon pickable visible boxes at end of axes
  float cr = r;
  sEndX    = Mesh::CreateBox("", cr, scene);
  sEndY    = sEndX->clone("");
  sEndZ    = sEndX->clone("");
  PolyhedronOptions polyhedronOptions(cr / 2.f);
  polyhedronOptions.type = 1;
  sEndAll = MeshBuilder::CreatePolyhedron("sEndAll", polyhedronOptions, scene);
  sEndXZ  = createTriangle("XZ", cr * 1.75f, scene);
  sEndZY  = sEndXZ->clone("ZY");
  sEndYX  = sEndXZ->clone("YX");

  sEndXZ->rotation().x = -1.57f;
  sEndZY->rotation().x = -1.57f;
  sEndYX->rotation().x = -1.57f;

  sEndX->setParent(sX.get());
  sEndY->setParent(sY.get());
  sEndZ->setParent(sZ.get());
  sEndXZ->setParent(sXZ.get());
  sEndZY->setParent(sZY.get());
  sEndYX->setParent(sYX.get());
  sEndAll->setParent(sAll.get());

  sEndX->position().z = l - cr / 2.f;
  sEndY->position().z = l - cr / 2.f;
  sEndZ->position().z = l - cr / 2.f;
  sEndX->material     = redMat;
  sEndY->material     = greenMat;
  sEndZ->material     = blueMat;
  sEndXZ->material    = redMat;
  sEndZY->material    = blueMat;
  sEndYX->material    = greenMat;
  sEndAll->material   = yellowMat;

  sEndX->renderingGroupId   = 1;
  sEndY->renderingGroupId   = 1;
  sEndZ->renderingGroupId   = 1;
  sEndXZ->renderingGroupId  = 1;
  sEndZY->renderingGroupId  = 1;
  sEndYX->renderingGroupId  = 1;
  sEndAll->renderingGroupId = 1;

  sEndX->isPickable   = false;
  sEndY->isPickable   = false;
  sEndZ->isPickable   = false;
  sEndXZ->isPickable  = false;
  sEndZY->isPickable  = false;
  sEndYX->isPickable  = false;
  sEndAll->isPickable = false;
}

void EditControl::setLocalAxes(const MeshPtr& mesh)
{
  auto meshMatrix = *mesh->getWorldMatrix();
  Vector3::FromArrayToRef(meshMatrix.asArray(), 0, localX);
  Vector3::FromArrayToRef(meshMatrix.asArray(), 4, localY);
  Vector3::FromArrayToRef(meshMatrix.asArray(), 8, localZ);
}

void EditControl::setLocal(bool l)
{
  if (local == l) {
    return;
  }
  local = l;
  if (local) {
    theParent->rotationQuaternion = mesh->rotationQuaternion();
  }
  else {
    theParent->rotationQuaternion = Quaternion::Identity();
  }
}

bool EditControl::isLocal() const
{
  return local;
}

void EditControl::setTransSnap(bool s)
{
  snapT = s;
}

void EditControl::setRotSnap(bool s)
{
  snapR = s;
}

void EditControl::setScaleSnap(bool s)
{
  snapS = s;
}

void EditControl::setTransSnapValue(float t)
{
  tSnap     = Vector3(t, t, t);
  transSnap = t;
}

void EditControl::setRotSnapValue(float r)
{
  rotSnap = r;
}

void EditControl::setScaleSnapValue(float r)
{
  scaleSnap = r;
}

void EditControl::setAxesScale()
{
  theParent->position().subtractToRef(mainCamera->position, toParent);
  Vector3::FromArrayToRef(mainCamera->getWorldMatrix()->asArray(), 8,
                          cameraNormal);
  // Get distance of axes from the camera plane - project camera to axes vector
  // on the camera normal
  const float parentOnNormal
    = Vector3::Dot(toParent, cameraNormal) / cameraNormal.length();
  const float s = parentOnNormal / distFromCamera;
  Vector3::FromFloatsToRef(s, s, s, theParent->scaling());
  Vector3::FromFloatsToRef(s, s, s, pALL->scaling());
}

float EditControl::getAngle(const Vector3& p1, const Vector3& p2,
                            const Vector3& p, const Vector3& cN)
{
  auto v1     = p1.subtract(p);
  auto v2     = p2.subtract(p);
  auto n      = Vector3::Cross(v1, v2);
  float angle = std::asin(n.length() / (v1.length() * v2.length()));
  if ((Vector3::Dot(n, cN) < 0.f)) {
    angle = -1.f * angle;
  }
  return angle;
}

void EditControl::createMaterials(Scene* scene)
{
  redMat = EditControl::getStandardMaterial("redMat", Color3::Red(), scene);
  greenMat
    = EditControl::getStandardMaterial("greenMat", Color3::Green(), scene);
  blueMat = EditControl::getStandardMaterial("blueMat", Color3::Blue(), scene);
  whiteMat
    = EditControl::getStandardMaterial("whiteMat", Color3::White(), scene);
  yellowMat
    = EditControl::getStandardMaterial("whiteMat", Color3::Yellow(), scene);
}

void EditControl::disposeMaterials()
{
  redMat->dispose();
  greenMat->dispose();
  blueMat->dispose();
  whiteMat->dispose();
  yellowMat->dispose();
}

StandardMaterialPtr EditControl::getStandardMaterial(const std::string& name,
                                                     const Color3& col,
                                                     Scene* scene)
{
  auto mat             = StandardMaterial::New(name, scene);
  mat->emissiveColor   = col;
  mat->diffuseColor    = Color3::Black();
  mat->specularColor   = Color3::Black();
  mat->backFaceCulling = false;
  return mat;
}

} // end of namespace Extensions
} // end of namespace BABYLON
