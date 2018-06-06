#version 450 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 vert_normal;
layout (location = 2) in vec2 vert_uv;

uniform mat4 MVP;
uniform mat4 M;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;

void main() {
  gl_Position = MVP * vec4(vert_pos, 1.0);
  frag_pos = vec3(M * vec4(vert_pos, 1.0));
  frag_normal = vec3(MVP * vec4(vert_normal, 0.0));
	frag_uv = vert_uv;
}
