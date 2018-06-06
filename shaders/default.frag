#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

uniform sampler2D iChannel0;

out vec4 frag_color;

void main() {
	frag_color = vec4(texture(iChannel0, frag_uv));
}
