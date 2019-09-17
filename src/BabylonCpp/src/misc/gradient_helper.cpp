#include <babylon/misc/gradient_helper.h>

namespace BABYLON {

void GradientHelper::GetCurrentGradient(
  float ratio, const std::vector<IValueGradient>& gradients,
  const std::function<void(const IValueGradient& current,
                           const IValueGradient& next, float scale)>&
    updateFunc)
{
  for (size_t gradientIndex = 0; gradientIndex < gradients.size() - 1;
       ++gradientIndex) {
    const auto& currentGradient = gradients[gradientIndex];
    const auto& nextGradient    = gradients[gradientIndex + 1];

    if (ratio >= currentGradient.gradient && ratio <= nextGradient.gradient) {
      const auto scale = (ratio - currentGradient.gradient)
                         / (nextGradient.gradient - currentGradient.gradient);
      updateFunc(currentGradient, nextGradient, scale);
      return;
    }
  }

  // Use last index if over
  const auto lastIndex = gradients.size() - 1;
  updateFunc(gradients[lastIndex], gradients[lastIndex], 1.f);
}

} // end of namespace BABYLON
