#ifndef BABYLON_BEHAVIORS_MESHES_FADE_IN_OUT_BEHAVIOR_H
#define BABYLON_BEHAVIORS_MESHES_FADE_IN_OUT_BEHAVIOR_H

#include <babylon/babylon_api.h>
#include <babylon/behaviors/behavior.h>

namespace BABYLON {

class AbstractMesh;
class Mesh;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using MeshPtr         = std::shared_ptr<Mesh>;

/**
 * @brief A behavior that when attached to a mesh will allow the mesh to fade in
 * and out.
 */
class BABYLON_SHARED_EXPORT FadeInOutBehavior : public Behavior<Mesh> {

public:
  /**
   * @brief Instatiates the FadeInOutBehavior.
   */
  FadeInOutBehavior();
  virtual ~FadeInOutBehavior(); // = default

  /**
   *  @brief The name of the behavior.
   */
  const char* name() const;

  /**
   * @brief Initializes the behavior.
   */
  void init() override;

  /**
   * @brief Attaches the fade behavior on the passed in mesh.
   * @param ownerNode The mesh that will be faded in/out once attached
   * @param predicate Predicate to use for pick filtering
   */
  void attach(const MeshPtr& ownerNode,
              const std::function<bool(const AbstractMeshPtr& m)>& predicate
              = nullptr) override;

  /**
   * @brief Detaches the behavior from the mesh.
   */
  void detach() override;

  /**
   * @brief Triggers the mesh to begin fading in or out.
   * @param value if the object should fade in or out (true to fade in)
   */
  void fadeIn(bool value);

private:
  void _update();
  void _setAllVisibility(const AbstractMeshPtr& mesh, float value);

public:
  /**
   * Time in milliseconds to delay before fading in (Default: 0)
   */
  float delay;

  /**
   * Time in milliseconds for the mesh to fade in (Default: 300)
   */
  float fadeInTime;

private:
  float _millisecondsPerFrame;
  bool _hovered;
  float _hoverValue;
  MeshPtr _ownerNode;

}; // end of class FadeInOutBehavior

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_MESHES_FADE_IN_OUT_BEHAVIOR_H
