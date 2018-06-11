#version 410 core
layout (location = 0) in vec3 vert_pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vert_pos, 1.0);
}

