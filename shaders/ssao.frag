#version 410 core

const int kernel_size = 64;
const float radius = 1.5;
const float bias = 0.025;

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

vec3 depth_to_world_space(float depth) {
  float z = depth * 2.0f - 1.0f;
  vec4 cp_pos = vec4(frag_uv * 2.0 - 1.0, depth, 1.0);
  vec4 ws_pos = invVP * cp_pos;
  ws_pos /= ws_pos.w;
  return (ws_pos.xyz);
}

void main() {
  vec3 color = vec3(0.0);
  vec2 noise_scale = screen_size / 4.0f;

  vec3 frag_pos = depth_to_world_space(texture(depth_tex, frag_uv).r);
  vec3 normal = texture(normal_tex, frag_uv).rgb;
  vec3 rand_vec = texture(noise_tex, frag_uv).rgb;

  vec3 tangent = normalize(rand_vec - normal * dot(rand_vec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);

  float occlusion = 0.0;
  for (int i = 0; i < kernel_size; ++i) {
    vec3 s = TBN * samples[i].xyz;
    s = frag_pos + s * radius;

    vec4 offset = P * vec4(s, 1.0f);
    offset /= offset.w;
    offset = offset * 0.5 + 0.5;
    float sample_depth = depth_to_world_space(texture(depth_tex, offset.xy).r).z;
    float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));
    occlusion += (sample_depth >= s.z + bias ? 1.0 : 0.0) * range_check;
  }

  occlusion = 1.0f - (occlusion / kernel_size);
  frag_color = occlusion;
}

