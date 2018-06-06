#version 450 core
layout (location = 2) in vec2 vert_uv;

in vec3 frag_pos;
in vec3 frag_normal;

//uniform sampler2D iChannel0;

out vec4 frag_color;

void main() {
	//frag_color = vec4(1.0, 0.0, 0.0, 1.0);
	frag_color = vec4(vert_uv.x + normalize(frag_normal), 1.0);
}
