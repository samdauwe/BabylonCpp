#ifndef BABYLON_MATERIALS_LIBRARY_GRID_LEGACYGRID_FRAGMENT_FX_H
#define BABYLON_MATERIALS_LIBRARY_GRID_LEGACYGRID_FRAGMENT_FX_H

namespace BABYLON {

extern const char* legacygridPixelShader;

const char* legacygridPixelShader
  = "uniform vec3 mainColor;\n"
    "uniform vec4 gridControl;\n"
    "\n"
    "void main(void) {\n"
    "  gl_FragColor = vec4(1, 1, 1, 0.1);\n"
    "  \n"
    "  #ifdef TRANSPARENT\n"
    "  // Min opacity as if there were no lines.\n"
    "  gl_FragColor = vec4(mainColor.rgb, 0.08);\n"
    "  #else\n"
    "  // Apply the color.\n"
    "  gl_FragColor = vec4(mainColor.rgb, 1.0);\n"
    "  #endif\n"
    "}\n";

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_LIBRARY_GRID_LEGACYGRID_FRAGMENT_FX_H
