#include <babylon/maths/path3d.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/maths/scalar.h>
#include <babylon/misc/tools.h>

namespace BABYLON {

Path3D::Path3D() = default;

Path3D::Path3D(const std::vector<Vector3>& iPath, const std::optional<Vector3>& firstNormal,
               bool raw, bool alignTangentsWithPath)
    : path{iPath}, _raw{raw}
{
  for (auto& vector : iPath) {
    _curve.emplace_back(vector);
  }

  _alignTangentsWithPath = alignTangentsWithPath;
  _compute(firstNormal, alignTangentsWithPath);
}

Path3D::Path3D(const Path3D& otherPath) = default;
Path3D::Path3D(Path3D&& otherPath)      = default;
Path3D& Path3D::operator=(const Path3D& otherPath) = default;
Path3D& Path3D::operator=(Path3D&& otherPath) = default;

Path3D::~Path3D() = default;

Path3D Path3D::copy() const
{
  return Path3D(*this);
}

std::unique_ptr<Path3D> Path3D::clone() const
{
  return std::make_unique<Path3D>(*this);
}

std::ostream& operator<<(std::ostream& os, const Path3D& path)
{
  os << "{\"Raw\":" << path._raw;
  os << ",\"Curve\":[";
  if (!path._curve.empty()) {
    for (unsigned int i = 0; i < path._curve.size() - 1; ++i) {
      os << path._curve[i] << ",";
    }
    os << path._curve.back();
  }
  os << "],\"Distances\":[";
  if (!path._distances.empty()) {
    for (unsigned int i = 0; i < path._distances.size() - 1; ++i) {
      os << path._distances[i] << ",";
    }
    os << path._distances.back();
  }
  os << "],\"Tangents\":[";
  if (!path._tangents.empty()) {
    for (unsigned int i = 0; i < path._tangents.size() - 1; ++i) {
      os << path._tangents[i] << ",";
    }
    os << path._tangents.back();
  }
  os << "],\"Normals\":[";
  if (!path._normals.empty()) {
    for (unsigned int i = 0; i < path._normals.size() - 1; ++i) {
      os << path._normals[i] << ",";
    }
    os << path._normals.back();
  }
  os << "],\"Binormals\":[";
  if (!path._binormals.empty()) {
    for (unsigned int i = 0; i < path._binormals.size() - 1; ++i) {
      os << path._binormals[i] << ",";
    }
    os << path._binormals.back();
  }
  os << "]}";
  return os;
}

std::vector<Vector3>& Path3D::getCurve()
{
  return _curve;
}

std::vector<Vector3>& Path3D::getPoints()
{
  return _curve;
}

float Path3D::length() const
{
  return _distances.back();
}

std::vector<Vector3>& Path3D::getTangents()
{
  return _tangents;
}

std::vector<Vector3>& Path3D::getNormals()
{
  return _normals;
}

std::vector<Vector3>& Path3D::getBinormals()
{
  return _binormals;
}

Float32Array& Path3D::getDistances()
{
  return _distances;
}

Vector3 Path3D::getPointAt(float position)
{
  return _updatePointAtData(position).point;
}

Vector3 Path3D::getTangentAt(float position, bool interpolated)
{
  _updatePointAtData(position, interpolated);
  return interpolated ?
           Vector3::TransformCoordinates(Vector3::Forward(), _pointAtData.interpolationMatrix) :
           _tangents[_pointAtData.previousPointArrayIndex];
}

Vector3 Path3D::getNormalAt(float position, bool interpolated)
{
  _updatePointAtData(position, interpolated);
  return interpolated ?
           Vector3::TransformCoordinates(Vector3::Right(), _pointAtData.interpolationMatrix) :
           _normals[_pointAtData.previousPointArrayIndex];
}

Vector3 Path3D::getBinormalAt(float position, bool interpolated)
{
  _updatePointAtData(position, interpolated);
  return interpolated ?
           Vector3::TransformCoordinates(Vector3::UpReadOnly(), _pointAtData.interpolationMatrix) :
           _binormals[_pointAtData.previousPointArrayIndex];
}

float Path3D::getDistanceAt(float position) const
{
  return length() * position;
}

size_t Path3D::getPreviousPointIndexAt(float position)
{
  _updatePointAtData(position);
  return _pointAtData.previousPointArrayIndex;
}

float Path3D::getSubPositionAt(float position)
{
  _updatePointAtData(position);
  return _pointAtData.subPosition;
}

float Path3D::getClosestPositionTo(const Vector3& target)
{
  auto smallestDistance = std::numeric_limits<float>::max();
  auto closestPosition  = 0.f;
  for (size_t i = 0; !_curve.empty() && i < _curve.size() - 1; ++i) {
    const auto point     = _curve[i + 0];
    const auto tangent   = _curve[i + 1].subtract(point).normalize();
    const auto subLength = _distances[i + 1] - _distances[i + 0];
    const auto subPosition
      = std::min(std::max(Vector3::Dot(tangent, target.subtract(point).normalize()), 0.f)
                   * Vector3::Distance(point, target) / subLength,
                 1.f);
    const auto distance
      = Vector3::Distance(point.add(tangent.scale(subPosition * subLength)), target);

    if (distance < smallestDistance) {
      smallestDistance = distance;
      closestPosition  = (_distances[i + 0] + subLength * subPosition) / length();
    }
  }
  return closestPosition;
}

Path3D Path3D::slice(float start, float end)
{
  if (start < 0.f) {
    start = 1.f - (start * -1.f) /* % 1.f */;
  }
  if (end < 0.f) {
    end = 1.f - (end * -1.f) /* % 1.f */;
  }
  if (start > end) {
    auto _start = start;
    start       = end;
    end         = _start;
  }
  auto curvePoints = getCurve();

  auto startPoint = getPointAt(start);
  auto startIndex = getPreviousPointIndexAt(start);

  auto endPoint = getPointAt(end);
  auto endIndex = getPreviousPointIndexAt(end) + 1;

  std::vector<Vector3> slicePoints;
  if (start != 0.f) {
    startIndex++;
    slicePoints.emplace_back(startPoint);
  }

  auto slice
    = stl_util::slice(curvePoints, static_cast<int>(startIndex), static_cast<int>(endIndex));
  stl_util::concat(slicePoints, slice);
  if (end != 1.f || start == 1.f) {
    slicePoints.emplace_back(endPoint);
  }
  return Path3D(slicePoints, getNormalAt(start), _raw, _alignTangentsWithPath);
}

Path3D& Path3D::update(const std::vector<Vector3>& iPath, const std::optional<Vector3>& firstNormal,
                       bool alignTangentsWithPath)
{
  if (_curve.size() < iPath.size()) {
    _curve.resize(iPath.size());
  }

  for (unsigned int p = 0; p < iPath.size(); ++p) {
    _curve[p].x = iPath[p].x;
    _curve[p].y = iPath[p].y;
    _curve[p].z = iPath[p].z;
  }
  _compute(firstNormal, alignTangentsWithPath);
  return *this;
}

void Path3D::_compute(const std::optional<Vector3>& firstNormal, bool alignTangentsWithPath)
{
  const auto l = _curve.size();

  _binormals.resize(l);
  _distances.resize(l);
  _normals.resize(l);
  _tangents.resize(l);

  // first and last tangents
  _tangents[0] = _getFirstNonNullVector(0);
  if (!_raw) {
    _tangents[0].normalize();
  }
  _tangents[l - 1] = _curve[l - 1].subtract(_curve[l - 2]);
  if (!_raw) {
    _tangents[l - 1].normalize();
  }

  // normals and binormals at first point : arbitrary vector with
  // _normalVector()
  const auto& tg0 = _tangents[0];
  const auto pp0  = _normalVector(tg0, firstNormal);
  _normals[0]     = pp0;
  if (!_raw) {
    _normals[0].normalize();
  }
  _binormals[0] = Vector3::Cross(tg0, _normals[0]);
  if (!_raw) {
    _binormals[0].normalize();
  }
  _distances[0] = 0.f;

  // normals and binormals : next points
  Vector3 prev;      // previous vector (segment)
  Vector3 cur;       // current vector (segment)
  Vector3 curTang;   // current tangent
  Vector3 prevNor;   // previous normal
  Vector3 prevBinor; // previous binormal

  for (unsigned int i = 1; i < l; ++i) {
    // tangents
    prev = _getLastNonNullVector(i);
    if (i < l - 1) {
      cur          = _getFirstNonNullVector(i);
      _tangents[i] = alignTangentsWithPath ? cur : prev.add(cur);
      _tangents[i].normalize();
    }
    _distances[i] = _distances[i - 1] + prev.length();

    // normals and binormals
    // http://www.cs.cmu.edu/afs/andrew/scs/cs/15-462/web/old/asst2camera.html
    curTang     = _tangents[i];
    prevBinor   = _binormals[i - 1];
    _normals[i] = Vector3::Cross(prevBinor, curTang);
    if (!_raw) {
      if (_normals[i].length() == 0.f) {
        prevNor     = _normals[i - 1];
        _normals[i] = prevNor;
      }
      else {
        _normals[i].normalize();
      }
    }
    _binormals[i] = Vector3::Cross(curTang, _normals[i]);
    if (!_raw) {
      _binormals[i].normalize();
    }
  }
  _pointAtData.id = std::nullopt;
}

Vector3 Path3D::_getFirstNonNullVector(unsigned int index)
{
  unsigned int i   = 1;
  Vector3 nNVector = _curve[index + i].subtract(_curve[index]);
  while (stl_util::almost_equal(nNVector.length(), 0.f) && index + i + 1 < _curve.size()) {
    ++i;
    nNVector = _curve[index + i].subtract(_curve[index]);
  }
  return nNVector;
}

Vector3 Path3D::_getLastNonNullVector(unsigned int index)
{
  unsigned int i   = 1;
  Vector3 nLVector = _curve[index].subtract(_curve[index - i]);
  while (stl_util::almost_equal(nLVector.length(), 0.f) && index > i + 1) {
    ++i;
    nLVector = _curve[index].subtract(_curve[index - i]);
  }
  return nLVector;
}

Vector3 Path3D::_normalVector(const Vector3& vt, const std::optional<Vector3>& va)
{
  Vector3 normal0;
  float tgl = vt.length();
  if (stl_util::almost_equal(tgl, 0.f)) {
    tgl = 1.f;
  }

  if (va == std::nullopt) {
    Vector3 point;
    if (!Scalar::WithinEpsilon(std::abs(vt.y) / tgl, 1.f,
                               Math::Epsilon)) { // search for a point in the plane
      point = Vector3(0.f, -1.f, 0.f);
    }
    else if (!Scalar::WithinEpsilon(std::abs(vt.x) / tgl, 1.f, Math::Epsilon)) {
      point = Vector3(1.f, 0.f, 0.f);
    }
    else if (!Scalar::WithinEpsilon(std::abs(vt.z) / tgl, 1.f, Math::Epsilon)) {
      point = Vector3(0.f, 0.f, 1.f);
    }
    else {
      point = Vector3::Zero();
    }
    normal0 = Vector3::Cross(vt, point);
  }
  else {
    normal0 = Vector3::Cross(vt, *va);
    Vector3::CrossToRef(normal0, vt, normal0);
  }
  normal0.normalize();
  return normal0;
}

Path3D::PointAtData& Path3D::_updatePointAtData(float position, bool interpolateTNB)
{
  // set an id for caching the result
  if (_pointAtData.id.has_value() && stl_util::almost_equal(_pointAtData.id.value(), position)) {
    if (!_pointAtData.interpolateReady) {
      _updateInterpolationMatrix();
    }
    return _pointAtData;
  }
  else {
    _pointAtData.id = position;
  }
  const auto& curvePoints = getPoints();

  // clamp position between 0.0 and 1.0
  if (position <= 0.f) {
    return _setPointAtData(0.f, 0.f, curvePoints[0], 0, interpolateTNB);
  }
  else if (position >= 1.f) {
    return _setPointAtData(1.f, 1.f, curvePoints.back(), curvePoints.size() - 1, interpolateTNB);
  }

  auto previousPoint = curvePoints[0];
  Vector3 currentPoint;
  auto currentLength = 0.f;
  auto targetLength  = position * length();

  for (size_t i = 1; i < curvePoints.size(); ++i) {
    currentPoint  = curvePoints[i];
    auto distance = Vector3::Distance(previousPoint, currentPoint);
    currentLength += distance;
    if (stl_util::almost_equal(currentLength, targetLength)) {
      return _setPointAtData(position, 1.f, currentPoint, i, interpolateTNB);
    }
    else if (currentLength > targetLength) {
      auto toLength = currentLength - targetLength;
      auto diff     = toLength / distance;
      auto dir      = previousPoint.subtract(currentPoint);
      auto point    = currentPoint.add(dir.scaleInPlace(diff));
      return _setPointAtData(position, 1.f - diff, point, i - 1, interpolateTNB);
    }
    previousPoint = currentPoint;
  }
  return _pointAtData;
}

Path3D::PointAtData& Path3D::_setPointAtData(float position, float subPosition,
                                             const Vector3& point, size_t parentIndex,
                                             bool interpolateTNB)
{
  _pointAtData.point                   = point;
  _pointAtData.position                = position;
  _pointAtData.subPosition             = subPosition;
  _pointAtData.previousPointArrayIndex = parentIndex;
  _pointAtData.interpolateReady        = interpolateTNB;

  if (interpolateTNB) {
    _updateInterpolationMatrix();
  }
  return _pointAtData;
}

void Path3D::_updateInterpolationMatrix()
{
  _pointAtData.interpolationMatrix = Matrix::Identity();
  auto parentIndex                 = _pointAtData.previousPointArrayIndex;

  if (parentIndex != _tangents.size() - 1) {
    auto index = parentIndex + 1;

    auto tangentFrom  = _tangents[parentIndex];
    auto normalFrom   = _normals[parentIndex];
    auto binormalFrom = _binormals[parentIndex];

    auto tangentTo  = _tangents[index];
    auto normalTo   = _normals[index];
    auto binormalTo = _binormals[index];

    auto quatFrom = Quaternion::RotationQuaternionFromAxis(normalFrom, binormalFrom, tangentFrom);
    auto quatTo   = Quaternion::RotationQuaternionFromAxis(normalTo, binormalTo, tangentTo);
    auto quatAt   = Quaternion::Slerp(quatFrom, quatTo, _pointAtData.subPosition);

    quatAt.toRotationMatrix(_pointAtData.interpolationMatrix);
  }
}

} // end of namespace BABYLON
