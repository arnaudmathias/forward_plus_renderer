#version 410 core

in vec2 frag_uv;

out float frag_color;

uniform sampler2D depth_tex;
uniform sampler2D normal_tex;
uniform sampler2D noise_tex;

layout (std140) uniform ssbo_data {
  uniform vec4 samples[64];
};

uniform mat4 P;
uniform mat4 invVP;
uniform vec2 screen_size;

void main() {
  vec3 color = vec3(0.0);
  vec2 noise_scale = screen_size / 4.0f;

  vec3 normal = texture(normal_tex, frag_uv).rgb;
  vec3 noise = texture(noise_tex, frag_uv).rgb;
  float depth = texture(depth_tex, frag_uv).r;

	frag_color = 1.0f;
  //frag_color = vec4(noise, 1.0f);
  //frag_color = normal.r;
}

