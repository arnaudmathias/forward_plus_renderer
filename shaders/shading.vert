#version 450 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 vert_normal;
layout (location = 2) in vec2 vert_uv;
layout (location = 3) in vec3 vert_tangent;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 M;
uniform vec3 view_pos;

out VS_OUT {
  vec2 frag_uv;
  mat3 TBN;
  vec3 ts_frag_pos;
  vec3 ts_view_pos;
} vs_out; 


void main() {
  gl_Position = MVP * vec4(vert_pos, 1.0);
  vec3 frag_pos = vec3(M * vec4(vert_pos, 1.0));

  mat3 normal_matrix = transpose(inverse(mat3(M)));
  vec3 N = normalize(vec3(normal_matrix * vert_normal));
  vec3 T = normalize(vec3(normal_matrix * vert_tangent));
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  vs_out.frag_uv = vert_uv;
  vs_out.TBN = transpose(mat3(T, B, N));    
  vs_out.ts_view_pos  = vs_out.TBN * view_pos;
  vs_out.ts_frag_pos = vs_out.TBN * frag_pos;
}
