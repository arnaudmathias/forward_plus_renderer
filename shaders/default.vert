#version 410 core
layout (location = 0) in vec4 vert_pos; // vec2 pos | vec2 uv

out vec2 frag_uv;

void main() {
	gl_Position = vec4(vert_pos.xy, 0.0f, 1.0);
	frag_uv = vert_pos.zw;
	frag_uv.y *= -1.0f;
}
