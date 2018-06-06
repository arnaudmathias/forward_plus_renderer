#version 410 core
layout(location = 0) in vec4 vertex_pos; // vec2 pos | vec2 uv

out vec2 frag_uv;

uniform mat4 proj;

void main() {
	gl_Position = proj * vec4(vertex_pos.xy, 0.0f, 1.0f);
	frag_uv = vertex_pos.zw;
}
