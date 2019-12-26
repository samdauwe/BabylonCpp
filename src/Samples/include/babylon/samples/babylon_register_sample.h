//
// Created by Pascal Thomet on 14/12/2019.
//

#ifndef BABYLONCPP_BABYLON_REGISTER_SAMPLE_H
#define BABYLONCPP_BABYLON_REGISTER_SAMPLE_H

// BABYLON_REGISTER_SAMPLE : registers an example in the Samples Index
#define  BABYLON_REGISTER_SAMPLE(categoryName, sampleClassName)                          \
std::shared_ptr<IRenderableScene> make_##sampleClassName(ICanvas* canvas)                \
{                                                                                        \
  return std::make_shared<sampleClassName>(canvas);                                      \
}

#endif // BABYLONCPP_BABYLON_REGISTER_SAMPLE_H
