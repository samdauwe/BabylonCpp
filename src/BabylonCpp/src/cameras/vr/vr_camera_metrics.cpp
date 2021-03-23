#include <babylon/cameras/vr/vr_camera_metrics.h>

#include <cmath>

#include <babylon/maths/matrix.h>

namespace BABYLON {

VRCameraMetrics::VRCameraMetrics()
    : hResolution{1280}
    , vResolution{800}
    , hScreenSize{0.149759993f}
    , vScreenSize{0.0935999975f}
    , vScreenCenter{0.0467999987f}
    , eyeToScreenDistance{0.0410000011f}
    , lensSeparationDistance{0.0635000020f}
    , interpupillaryDistance{0.0640000030f}
    , distortionK{{1.f, 0.219999999f, 0.239999995f, 0.f}}
    , chromaAbCorrection{{0.995999992f, -0.00400000019f, 1.01400006f, 0.f}}
    , postProcessScaleFactor{1.714605507808412f}
    , lensCenterOffset{0.151976421f}
    , compensateDistortion{true}
    , multiviewEnabled{false}
{
}

VRCameraMetrics::VRCameraMetrics(VRCameraMetrics& other) = default;

VRCameraMetrics::VRCameraMetrics(VRCameraMetrics&& other) = default;

VRCameraMetrics& VRCameraMetrics::operator=(const VRCameraMetrics& other) = default;

VRCameraMetrics& VRCameraMetrics::operator=(VRCameraMetrics&& other) = default;

VRCameraMetrics::~VRCameraMetrics() = default;

float VRCameraMetrics::aspectRatio() const
{
  return static_cast<float>(hResolution) / (2.f * static_cast<float>(vResolution));
}

float VRCameraMetrics::aspectRatioFov() const
{
  return (2.f * std::atan((postProcessScaleFactor * vScreenSize) / (2.f * eyeToScreenDistance)));
}

Matrix VRCameraMetrics::leftHMatrix() const
{
  float meters = (hScreenSize / 4.f) - (lensSeparationDistance / 2.f);
  float h      = (4.f * meters) / hScreenSize;

  return Matrix::Translation(h, 0.f, 0.f);
}

Matrix VRCameraMetrics::rightHMatrix() const
{
  const auto meters = (hScreenSize / 4.f) - (lensSeparationDistance / 2.f);
  const auto h      = (4.f * meters) / hScreenSize;

  return Matrix::Translation(-h, 0.f, 0.f);
}

Matrix VRCameraMetrics::leftPreViewMatrix() const
{
  return Matrix::Translation(0.5f * interpupillaryDistance, 0.f, 0.f);
}

Matrix VRCameraMetrics::rightPreViewMatrix() const
{
  return Matrix::Translation(-0.5f * interpupillaryDistance, 0.f, 0.f);
}

VRCameraMetrics VRCameraMetrics::GetDefault()
{
  return VRCameraMetrics();
}

} // end of namespace BABYLON
