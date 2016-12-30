#ifndef BABYLON_LENSFLARE_LENS_FLARE_H
#define BABYLON_LENSFLARE_LENS_FLARE_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT LensFlare : public IDisposable {

public:
  template <typename... Ts>
  static LensFlare* New(Ts&&... args);
  virtual ~LensFlare();

  void dispose(bool doNotRecurse = false) override;

protected:
  LensFlare(float size, const Vector3& position,
            const Color3& color       = Color3(1.f, 1.f, 1.f),
            const std::string& imgUrl = "", LensFlareSystem* system = nullptr);

public:
  float size;
  Vector3 position;
  Color3 color;
  Texture* texture;
  int alphaMode;

private:
  LensFlareSystem* _system;

}; // end of class LensFlare

} // end of namespace BABYLON

#endif // end of BABYLON_LENSFLARE_LENS_FLARE_H
