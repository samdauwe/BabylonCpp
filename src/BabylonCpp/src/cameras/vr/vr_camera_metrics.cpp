#include <babylon/cameras/vr/vr_camera_metrics.h>

#include <babylon/math/matrix.h>

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
{
}

VRCameraMetrics::VRCameraMetrics(VRCameraMetrics& other)
    : hResolution{other.hResolution}
    , vResolution{other.vResolution}
    , hScreenSize{other.hScreenSize}
    , vScreenSize{other.vScreenSize}
    , vScreenCenter{other.vScreenCenter}
    , eyeToScreenDistance{other.eyeToScreenDistance}
    , lensSeparationDistance{other.lensSeparationDistance}
    , interpupillaryDistance{other.interpupillaryDistance}
    , distortionK{other.distortionK}
    , chromaAbCorrection{other.chromaAbCorrection}
    , postProcessScaleFactor{other.postProcessScaleFactor}
    , lensCenterOffset{other.lensCenterOffset}
    , compensateDistortion{other.compensateDistortion}
{
}

VRCameraMetrics::VRCameraMetrics(VRCameraMetrics&& other)
    : hResolution{::std::move(other.hResolution)}
    , vResolution{::std::move(other.vResolution)}
    , hScreenSize{::std::move(other.hScreenSize)}
    , vScreenSize{::std::move(other.vScreenSize)}
    , vScreenCenter{::std::move(other.vScreenCenter)}
    , eyeToScreenDistance{::std::move(other.eyeToScreenDistance)}
    , lensSeparationDistance{::std::move(other.lensSeparationDistance)}
    , interpupillaryDistance{::std::move(other.interpupillaryDistance)}
    , distortionK{::std::move(other.distortionK)}
    , chromaAbCorrection{::std::move(other.chromaAbCorrection)}
    , postProcessScaleFactor{::std::move(other.postProcessScaleFactor)}
    , lensCenterOffset{::std::move(other.lensCenterOffset)}
    , compensateDistortion{::std::move(other.compensateDistortion)}
{
}

VRCameraMetrics& VRCameraMetrics::operator=(const VRCameraMetrics& other)
{
  if (&other != this) {
    hResolution            = other.hResolution;
    vResolution            = other.vResolution;
    hScreenSize            = other.hScreenSize;
    vScreenSize            = other.vScreenSize;
    vScreenCenter          = other.vScreenCenter;
    eyeToScreenDistance    = other.eyeToScreenDistance;
    lensSeparationDistance = other.lensSeparationDistance;
    interpupillaryDistance = other.interpupillaryDistance;
    distortionK            = other.distortionK;
    chromaAbCorrection     = other.chromaAbCorrection;
    postProcessScaleFactor = other.postProcessScaleFactor;
    lensCenterOffset       = other.lensCenterOffset;
    compensateDistortion   = other.compensateDistortion;
  }

  return *this;
}

VRCameraMetrics& VRCameraMetrics::operator=(VRCameraMetrics&& other)
{
  if (&other != this) {
    hResolution            = ::std::move(other.hResolution);
    vResolution            = ::std::move(other.vResolution);
    hScreenSize            = ::std::move(other.hScreenSize);
    vScreenSize            = ::std::move(other.vScreenSize);
    vScreenCenter          = ::std::move(other.vScreenCenter);
    eyeToScreenDistance    = ::std::move(other.eyeToScreenDistance);
    lensSeparationDistance = ::std::move(other.lensSeparationDistance);
    interpupillaryDistance = ::std::move(other.interpupillaryDistance);
    distortionK            = ::std::move(other.distortionK);
    chromaAbCorrection     = ::std::move(other.chromaAbCorrection);
    postProcessScaleFactor = ::std::move(other.postProcessScaleFactor);
    lensCenterOffset       = ::std::move(other.lensCenterOffset);
    compensateDistortion   = ::std::move(other.compensateDistortion);
  }

  return *this;
}

VRCameraMetrics::~VRCameraMetrics()
{
}

float VRCameraMetrics::aspectRatio() const
{
  return static_cast<float>(hResolution)
         / (2.f * static_cast<float>(vResolution));
}

float VRCameraMetrics::aspectRatioFov() const
{
  return (2.f
          * ::std::atan((postProcessScaleFactor * vScreenSize)
                        / (2.f * eyeToScreenDistance)));
}

Matrix VRCameraMetrics::leftHMatrix() const
{
  float meters = (hScreenSize / 4.f) - (lensSeparationDistance / 2.f);
  float h      = (4.f * meters) / hScreenSize;

  return Matrix::Translation(h, 0.f, 0.f);
}

Matrix VRCameraMetrics::rightHMatrix() const
{
  float meters = (hScreenSize / 4.f) - (lensSeparationDistance / 2.f);
  float h      = (4.f * meters) / hScreenSize;

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
