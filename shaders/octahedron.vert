#version 410 core
layout (location = 0) in vec3 vert_pos;

out vec2 frag_uv;
uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vert_pos.xyz, 1.0);
}

