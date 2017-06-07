#include <babylon/extensions/editcontrol/edit_control.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/math/axis.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {
namespace Extensions {

EditControl::EditControl(Mesh* mesh, Camera* camera, ICanvas* canvas,
                         float scale)
    : pDown{false}
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
{
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

void EditControl::switchTo(Mesh* iMesh)
{
  iMesh->computeWorldMatrix(true);
  mesh = iMesh;
  theParent->setRotationQuaternion(iMesh->rotationQuaternion());
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

void EditControl::detach()
{
}

void EditControl::disposeAll()
{
  theParent->dispose();
  disposeMaterials();
}

void EditControl::onPointerDown(const Event& evt)
{
}

bool EditControl::isEditing() const
{
  return editing;
}

void EditControl::detachControl(Camera* cam, ICanvas* can)
{
  cam->detachControl(can);
}

void EditControl::onPointerOver()
{
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
    static_cast<LinesMesh*>(mesh->getChildren()[0])->color = savedCol;
  }
  else {
    auto childs = mesh->getChildren();
    if (!childs.empty()) {
      static_cast<Mesh*>(childs[0])->setMaterial(savedMat);
    }
    else {
      mesh->setMaterial(savedMat);
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

  Vector3 diff = newPos.subtract(prevPos);
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

Mesh* EditControl::getPickPlane(Mesh* axis)
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
  else
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

void EditControl::transWithSnap(Mesh* mesh, Vector3& trans, bool local)
{
  if (snapT) {
    bool snapit = false;
    snapTV.addInPlace(trans);
    if (std::abs(snapTV.x) > (tSnap.x / mesh->scaling().x)) {
      if (snapTV.x > 0)
        trans.x = tSnap.x;
      else
        trans.x = -tSnap.x;
      trans.x   = trans.x / mesh->scaling().x;
      snapit    = true;
    }
    if (std::abs(snapTV.y) > (tSnap.y / mesh->scaling().y)) {
      if (snapTV.y > 0)
        trans.y = tSnap.y;
      else
        trans.y = -tSnap.y;
      trans.y   = trans.y / mesh->scaling().y;
      snapit    = true;
    }
    if (std::abs(snapTV.z) > (tSnap.z / mesh->scaling().z)) {
      if (snapTV.z > 0)
        trans.z = tSnap.z;
      else
        trans.z = -tSnap.z;
      trans.z   = trans.z / mesh->scaling().z;
      snapit    = true;
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

void EditControl::scaleWithSnap(Mesh* mesh, Vector3& p)
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

void EditControl::doRotation(Mesh* mesh, Mesh* axis, const Vector3& newPos)
{
  auto cN = Vector3::TransformNormal(Axis::Z, *mainCamera->getWorldMatrix());
  auto angle
    = EditControl::getAngle(prevPos, newPos, mesh->getAbsolutePivotPoint(), cN);

  if (axis == rX) {
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
        auto axis = Axis::X;
        mesh->rotate(axis, angle, Space::LOCAL);
      }
      else {
        auto axis = Vector3(cN.x, 0, 0);
        mesh->rotate(axis, angle, Space::WORLD);
      }
    }
  }
  else if (axis == rY) {
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
        auto axis = Axis::Y;
        mesh->rotate(axis, angle, Space::LOCAL);
      }
      else {
        auto axis = Vector3(0.f, cN.y, 0.f);
        mesh->rotate(axis, angle, Space::WORLD);
      }
    }
  }
  else if (axis == rZ) {
    if (snapR) {
      snapRZ += angle;
      angle = 0.f;
      if (std::abs(snapRZ) >= rotSnap) {
        if (snapRZ > 0)
          angle = rotSnap;
        else
          angle = -rotSnap;
        snapRZ  = 0;
      }
    }
    if (!stl_util::almost_equal(angle, 0.f)) {
      if (local) {
        if (Vector3::Dot(localZ, cN) < 0) {
          angle = -1.f * angle;
        }
        auto axis = Axis::Z;
        mesh->rotate(axis, angle, Space::LOCAL);
      }
      else {
        auto axis = Vector3(0.f, 0.f, cN.z);
        mesh->rotate(axis, angle, Space::WORLD);
      }
    }
  }
  else if (axis == rAll) {
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
      mesh->setRotation(mesh->rotationQuaternion().toEulerAngles());
      mesh->nullifyRotationQuaternion();
    }
  }
}

std::tuple<Vector3, bool> EditControl::getPosOnPickPlane()
{
  auto pickinfo
    = scene->pick(scene->pointerX(), scene->pointerY(),
                  [this](AbstractMesh* mesh) { return mesh == pickPlane; },
                  false, mainCamera);
  if ((pickinfo->hit)) {
    return {pickinfo->pickedPoint, true};
  }
  else {
    return {Vector3::Zero(), false};
  }
}

void EditControl::hideBaxis()
{
  bXaxis->visibility = 0;
  bYaxis->visibility = 0;
  bZaxis->visibility = 0;
}

} // end of namespace Extensions
} // end of namespace BABYLON
