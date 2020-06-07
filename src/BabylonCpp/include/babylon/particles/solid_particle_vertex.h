#ifndef BABYLON_PARTICLES_SOLID_PARTICLE_VERTEX_H
#define BABYLON_PARTICLES_SOLID_PARTICLE_VERTEX_H

#include <babylon/babylon_api.h>
#include <babylon/maths/color4.h>
#include <babylon/maths/vector2.h>
#include <babylon/maths/vector3.h>

namespace BABYLON {

/**
 * @brief Represents a solid particle vertex
 */
class BABYLON_SHARED_EXPORT SolidParticleVertex {

  SolidParticleVertex();
  ~SolidParticleVertex() = default;

protected:
  // Getters and Setters for back-compatibility
  /** Vertex x coordinate */
  float get_x() const;
  void set_x(float val);
  /** Vertex y coordinate */
  float get_y() const;
  void set_y(float val);
  /** Vertex z coordinate */
  float get_z() const;
  void set_z(float val);

public:
  /**
   * Vertex position
   */
  Vector3 position;

  /**
   * Vertex color
   */
  Color4 color;

  /**
   * Vertex UV
   */
  Vector2 uv;

  /**
   * Vertex x coordinate
   */
  Property<SolidParticleVertex, float> x;

  /**
   * Vertex y coordinate
   */
  Property<SolidParticleVertex, float> y;

  /**
   * Vertex z coordinate
   */
  Property<SolidParticleVertex, float> z;

}; // end of class SolidParticleVertex

} // end of namespace BABYLON

#endif // end of BABYLON_PARTICLES_SOLID_PARTICLE_VERTEX_H
