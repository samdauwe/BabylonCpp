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

} // end of namespace BABYLON
