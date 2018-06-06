#version 410 core
layout(location = 0) in vec3 vertex_pos;

uniform mat4 MVP;

void main() {
	gl_Position = MVP * vec4(vertex_pos.xyz, 1.0);
	//frag_uv = vertex_pos.zw;
}
