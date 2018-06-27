#version 410 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 vert_normal;
layout (location = 2) in vec2 vert_uv;

uniform mat4 MVP;

void main() {
  gl_Position = MVP * vec4(vert_pos, 1.0);
}
