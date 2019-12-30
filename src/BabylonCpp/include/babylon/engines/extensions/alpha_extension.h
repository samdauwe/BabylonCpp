#ifndef BABYLON_ENGINES_EXTENSIONS_ALPHA_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_ALPHA_EXTENSION_H

#include <babylon/babylon_api.h>
#include <babylon/meshes/abstract_mesh_constants.h>

namespace BABYLON {

class ThinEngine;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT AlphaExtension {

public:
  AlphaExtension(ThinEngine* engine);
  ~AlphaExtension();

  /**
   * @brief Sets alpha constants used by some alpha blending modes.
   * @param r defines the red component
   * @param g defines the green component
   * @param b defines the blue component
   * @param a defines the alpha component
   */
  void setAlphaConstants(float r, float g, float b, float a);

  /**
   * @brief Sets the current alpha mode.
   * @param mode defines the mode to use (one of the Engine.ALPHA_XXX)
   * @param noDepthWriteChange defines if depth writing state should remains unchanged (false by
   * default)
   * @see http://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered
   */
  void setAlphaMode(unsigned int mode, bool noDepthWriteChange = false);

  /**
   * @brief Gets the current alpha mode.
   * @see http://doc.babylonjs.com/resources/transparency_and_how_meshes_are_rendered
   * @returns the current alpha mode
   */
  unsigned int getAlphaMode() const;

  /**
   * @brief Sets the current alpha equation.
   * @param equation defines the equation to use (one of the Engine.ALPHA_EQUATION_XXX)
   */
  void setAlphaEquation(unsigned int equation);

  /**
   * @brief Gets the current alpha equation.
   * @returns the current alpha equation
   */
  unsigned int getAlphaEquation() const;

private:
  ThinEngine* _this;

}; // end of class AlphaExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_ALPHA_EXTENSION_H
