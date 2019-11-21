#ifndef BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
#define BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H

namespace BABYLON {

extern const char* cloudProceduralTexturePixelShader;

const char* cloudProceduralTexturePixelShader
  = R"ShaderCode(

#ifdef GL_ES
  precision highp float;
#endif

varying vec2 vUV;

uniform vec4 skyColor;
uniform vec4 cloudColor;

float rand(vec2 n) {
    return fract(cos(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
    const vec2 d = vec2(0.0, 1.0);
    vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
    return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float fbm(vec2 n) {
    float total = 0.0, amplitude = 1.0;
    for (int i = 0; i < 4; i++) {
        total += noise(n) * amplitude;
        n += n;
        amplitude *= 0.5;
    }
    return total;
}

void main() {

    vec2 p = vUV * 12.0;
    vec4 c = mix(skyColor, cloudColor, fbm(p));
    gl_FragColor = c;

}


)ShaderCode";
} // end of namespace BABYLON

#endif // end of BABYLON_PROCEDURAL_TEXTURES_LIBRARY_CLOUD_CLOUD_PROCEDURAL_TEXTURE_FRAGMENT_FX_H
