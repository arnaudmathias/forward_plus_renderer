#version 410 core

out vec4 frag_color;

uniform vec3 color;

void main() {
	frag_color = vec4(pow(color, vec3(2.2)), 1.0);	
}

