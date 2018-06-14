#version 410 core

in vec2 frag_uv;

out vec4 frag_color;

uniform sampler2D hdr_tex;

vec3 tone_mapping(vec3 x) {
  const float exposure = 1.5f;
  const float gamma = 2.2;

  vec3 mapped = vec3(1.0) - exp(-x * exposure);
  mapped = pow(mapped, vec3(1.0 / gamma));
  return (mapped);
}

void main() {
  vec3 color = texture(hdr_tex, frag_uv).rgb;
  color = tone_mapping(color);
  frag_color = vec4(color, 1.0f);	
}

