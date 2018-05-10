#ifndef BABYLON_SHADERS_DEPTH_OF_FIELD_MERGE_FRAGMENT_FX_H
#define BABYLON_SHADERS_DEPTH_OF_FIELD_MERGE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* depthOfFieldMergePixelShader;

const char* depthOfFieldMergePixelShader
  = "uniform sampler2D textureSampler;\n"
    "varying vec2 vUV;\n"
    "\n"
    "uniform sampler2D circleOfConfusionSampler;\n"
    "uniform sampler2D blurStep0;\n"
    "\n"
    "#if BLUR_LEVEL > 0\n"
    "uniform sampler2D blurStep1;\n"
    "#endif\n"
    "#if BLUR_LEVEL > 1\n"
    "uniform sampler2D blurStep2;\n"
    "#endif\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  float coc = texture2D(circleOfConfusionSampler, vUV).r;\n"
    "#if BLUR_LEVEL == 0\n"
    "  vec4 original = texture2D(textureSampler, vUV);\n"
    "  vec4 blurred0 = texture2D(blurStep0, vUV);\n"
    "  gl_FragColor = mix(original, blurred0, coc);\n"
    "#endif\n"
    "#if BLUR_LEVEL == 1\n"
    "  if(coc < 0.5){\n"
    "  vec4 original = texture2D(textureSampler, vUV);\n"
    "  vec4 blurred1 = texture2D(blurStep1, vUV);\n"
    "  gl_FragColor = mix(original, blurred1, coc/0.5);\n"
    "  }else{\n"
    "  vec4 blurred0 = texture2D(blurStep0, vUV);   \n"
    "  vec4 blurred1 = texture2D(blurStep1, vUV);\n"
    "  gl_FragColor = mix(blurred1, blurred0, (coc-0.5)/0.5);\n"
    "  }\n"
    "#endif\n"
    "#if BLUR_LEVEL == 2\n"
    "  if(coc < 0.33){\n"
    "  vec4 original = texture2D(textureSampler, vUV);\n"
    "  vec4 blurred2 = texture2D(blurStep2, vUV);\n"
    "  gl_FragColor = mix(original, blurred2, coc/0.33);\n"
    "  }else if(coc < 0.66){\n"
    "  vec4 blurred1 = texture2D(blurStep1, vUV);\n"
    "  vec4 blurred2 = texture2D(blurStep2, vUV);\n"
    "  gl_FragColor = mix(blurred2, blurred1, (coc-0.33)/0.33);\n"
    "  }else{\n"
    "  vec4 blurred0 = texture2D(blurStep0, vUV);\n"
    "  vec4 blurred1 = texture2D(blurStep1, vUV);\n"
    "  gl_FragColor = mix(blurred1, blurred0, (coc-0.66)/0.34);\n"
    "  }\n"
    "#endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_SHADERS_DEPTH_OF_FIELD_MERGE_FRAGMENT_FX_H
