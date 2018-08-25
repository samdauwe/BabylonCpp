#include <babylon/math/path3d.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/math/scalar.h>
#include <babylon/tools/tools.h>

namespace BABYLON {

Path3D::Path3D()
{
}

Path3D::Path3D(const vector_t<Vector3>& iPath,
               const nullable_t<Vector3>& firstNormal, bool raw)
    : path{iPath}, _raw{raw}
{
  for (auto& vector : iPath) {
    _curve.emplace_back(vector);
  }
  _compute(firstNormal);
}

Path3D::Path3D(const Path3D& otherPath)
    : path{otherPath.path}
    , _curve{otherPath._curve}
    , _distances{otherPath._distances}
    , _tangents{otherPath._tangents}
    , _normals{otherPath._normals}
    , _binormals{otherPath._binormals}
    , _raw{otherPath._raw}
{
}

Path3D::Path3D(Path3D&& otherPath)
    : path{otherPath.path}
    , _curve{::std::move(otherPath._curve)}
    , _distances{::std::move(otherPath._distances)}
    , _tangents{::std::move(otherPath._tangents)}
    , _normals{::std::move(otherPath._normals)}
    , _binormals{::std::move(otherPath._binormals)}
    , _raw{::std::move(otherPath._raw)}
{
}

Path3D& Path3D::operator=(const Path3D& otherPath)
{
  if (&otherPath != this) {
    path       = otherPath.path;
    _curve     = otherPath._curve;
    _distances = otherPath._distances;
    _tangents  = otherPath._tangents;
    _normals   = otherPath._normals;
    _binormals = otherPath._binormals;
    _raw       = otherPath._raw;
  }

  return *this;
}

Path3D& Path3D::operator=(Path3D&& otherPath)
{
  if (&otherPath != this) {
    path       = ::std::move(otherPath.path);
    _curve     = ::std::move(otherPath._curve);
    _distances = ::std::move(otherPath._distances);
    _tangents  = ::std::move(otherPath._tangents);
    _normals   = ::std::move(otherPath._normals);
    _binormals = ::std::move(otherPath._binormals);
    _raw       = ::std::move(otherPath._raw);
  }

  return *this;
}

Path3D::~Path3D()
{
}

Path3D Path3D::copy() const
{
  return Path3D(*this);
}

unique_ptr_t<Path3D> Path3D::clone() const
{
  return ::std::make_unique<Path3D>(*this);
}

std::ostream& operator<<(std::ostream& os, const Path3D& path)
{
  os << "{\"Raw\":" << path._raw;
  os << ",\"Curve\":[";
  if (path._curve.size() > 0) {
    for (unsigned int i = 0; i < path._curve.size() - 1; ++i) {
      os << path._curve[i] << ",";
    }
    os << path._curve.back();
  }
  os << "],\"Distances\":[";
  if (path._distances.size() > 0) {
    for (unsigned int i = 0; i < path._distances.size() - 1; ++i) {
      os << path._distances[i] << ",";
    }
    os << path._distances.back();
  }
  os << "],\"Tangents\":[";
  if (path._tangents.size() > 0) {
    for (unsigned int i = 0; i < path._tangents.size() - 1; ++i) {
      os << path._tangents[i] << ",";
    }
    os << path._tangents.back();
  }
  os << "],\"Normals\":[";
  if (path._normals.size() > 0) {
    for (unsigned int i = 0; i < path._normals.size() - 1; ++i) {
      os << path._normals[i] << ",";
    }
    os << path._normals.back();
  }
  os << "],\"Binormals\":[";
  if (path._binormals.size() > 0) {
    for (unsigned int i = 0; i < path._binormals.size() - 1; ++i) {
      os << path._binormals[i] << ",";
    }
    os << path._binormals.back();
  }
  os << "]}";
  return os;
}

vector_t<Vector3>& Path3D::getCurve()
{
  return _curve;
}

vector_t<Vector3>& Path3D::getTangents()
{
  return _tangents;
}

vector_t<Vector3>& Path3D::getNormals()
{
  return _normals;
}

vector_t<Vector3>& Path3D::getBinormals()
{
  return _binormals;
}

Float32Array& Path3D::getDistances()
{
  return _distances;
}

Path3D& Path3D::update(const vector_t<Vector3>& path,
                       const nullable_t<Vector3>& firstNormal)
{
  for (unsigned int p = 0; p < path.size(); ++p) {
    _curve[p].x = path[p].x;
    _curve[p].y = path[p].y;
    _curve[p].z = path[p].z;
  }
  _compute(firstNormal);
  return *this;
}

void Path3D::_compute(const nullable_t<Vector3>& firstNormal)
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
  const auto& tg0   = _tangents[0];
  const Vector3 pp0 = _normalVector(_curve[0], tg0, firstNormal);
  _normals[0]       = pp0;
  if (!_raw) {
    _normals[0].normalize();
  }
  _binormals[0] = Vector3::Cross(tg0, _normals[0]);
  if (!_raw) {
    _binormals[0].normalize();
  }
  _distances[0] = 0.f;

  // normals and binormals : next points
  Vector3 prev;    // previous vector (segment)
  Vector3 cur;     // current vector (segment)
  Vector3 curTang; // current tangent
  // previous normal
  Vector3 prevBinor; // previous binormal

  for (unsigned int i = 1; i < l; ++i) {
    // tangents
    prev = _getLastNonNullVector(i);
    if (i < l - 1) {
      cur          = _getFirstNonNullVector(i);
      _tangents[i] = prev.add(cur);
      _tangents[i].normalize();
    }
    _distances[i] = _distances[i - 1] + prev.length();

    // normals and binormals
    // http://www.cs.cmu.edu/afs/andrew/scs/cs/15-462/web/old/asst2camera.html
    curTang     = _tangents[i];
    prevBinor   = _binormals[i - 1];
    _normals[i] = Vector3::Cross(prevBinor, curTang);
    if (!_raw) {
      _normals[i].normalize();
    }
    _binormals[i] = Vector3::Cross(curTang, _normals[i]);
    if (!_raw) {
      _binormals[i].normalize();
    }
  }
}

Vector3 Path3D::_getFirstNonNullVector(unsigned int index)
{
  unsigned int i   = 1;
  Vector3 nNVector = _curve[index + i].subtract(_curve[index]);
  while (stl_util::almost_equal(nNVector.length(), 0.f)
         && index + i + 1 < _curve.size()) {
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

Vector3 Path3D::_normalVector(const Vector3& /*v0*/, const Vector3& vt,
                              const nullable_t<Vector3>& va)
{
  Vector3 normal0;
  float tgl = vt.length();
  if (stl_util::almost_equal(tgl, 0.f)) {
    tgl = 1.f;
  }

  if (va == nullopt_t) {
    Vector3 point;
    if (!Scalar::WithinEpsilon(
          ::std::abs(vt.y) / tgl, 1.f,
          Math::Epsilon)) { // search for a point in the plane
      point = Vector3(0.f, -1.f, 0.f);
    }
    else if (!Scalar::WithinEpsilon(::std::abs(vt.x) / tgl, 1.f,
                                    Math::Epsilon)) {
      point = Vector3(1.f, 0.f, 0.f);
    }
    else if (!Scalar::WithinEpsilon(::std::abs(vt.z) / tgl, 1.f,
                                    Math::Epsilon)) {
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

} // end of namespace BABYLON
