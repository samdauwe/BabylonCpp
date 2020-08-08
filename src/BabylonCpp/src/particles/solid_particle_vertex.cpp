#include <babylon/particles/solid_particle_vertex.h>

namespace BABYLON {

SolidParticleVertex::SolidParticleVertex()
    : x{this, &SolidParticleVertex::get_x, &SolidParticleVertex::set_x}
    , y{this, &SolidParticleVertex::get_y, &SolidParticleVertex::set_y}
    , z{this, &SolidParticleVertex::get_z, &SolidParticleVertex::set_z}
{
  position = Vector3::Zero();
  color    = Color4(1.f, 1.f, 1.f, 1.f);
  uv       = Vector2::Zero();
}

SolidParticleVertex::SolidParticleVertex(const SolidParticleVertex& otherSolidParticleVertex)
    : position{otherSolidParticleVertex.position}
    , color{otherSolidParticleVertex.color}
    , uv{otherSolidParticleVertex.uv}
    , x{this, &SolidParticleVertex::get_x, &SolidParticleVertex::set_x}
    , y{this, &SolidParticleVertex::get_y, &SolidParticleVertex::set_y}
    , z{this, &SolidParticleVertex::get_z, &SolidParticleVertex::set_z}
{
}

SolidParticleVertex::SolidParticleVertex(SolidParticleVertex&& otherSolidParticleVertex)
    : position{std::move(otherSolidParticleVertex.position)}
    , color{std::move(otherSolidParticleVertex.color)}
    , uv{std::move(otherSolidParticleVertex.uv)}
    , x{this, &SolidParticleVertex::get_x, &SolidParticleVertex::set_x}
    , y{this, &SolidParticleVertex::get_y, &SolidParticleVertex::set_y}
    , z{this, &SolidParticleVertex::get_z, &SolidParticleVertex::set_z}
{
}

SolidParticleVertex&
SolidParticleVertex::operator=(const SolidParticleVertex& otherSolidParticleVertex)
{
  if (this != &otherSolidParticleVertex) {
    position = otherSolidParticleVertex.position;
    color    = otherSolidParticleVertex.color;
    uv       = otherSolidParticleVertex.uv;
  }

  return *this;
}

SolidParticleVertex& SolidParticleVertex::operator=(SolidParticleVertex&& otherSolidParticleVertex)
{
  if (this != &otherSolidParticleVertex) {
    position = std::move(otherSolidParticleVertex.position);
    color    = std::move(otherSolidParticleVertex.color);
    uv       = std::move(otherSolidParticleVertex.uv);
  }

  return *this;
}

float SolidParticleVertex::get_x() const
{
  return position.x;
}

void SolidParticleVertex::set_x(float val)
{
  position.x = val;
}

float SolidParticleVertex::get_y() const
{
  return position.y;
}

void SolidParticleVertex::set_y(float val)
{
  position.y = val;
}

float SolidParticleVertex::get_z() const
{
  return position.z;
}

void SolidParticleVertex::set_z(float val)
{
  position.z = val;
}

bool operator==(const SolidParticleVertex& lhs, const SolidParticleVertex& rhs)
{
  return (lhs.position == rhs.position) && (lhs.color == rhs.color) && (lhs.uv == rhs.uv);
}

bool operator!=(const SolidParticleVertex& lhs, const SolidParticleVertex& rhs)
{
  return !operator==(lhs, rhs);
}

} // end of namespace BABYLON
