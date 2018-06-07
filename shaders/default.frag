#version 450 core

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

uniform sampler2D sdiffuse;
uniform sampler2D sspecular;
uniform sampler2D sbump;

out vec4 frag_color;

void main() {
	vec3 light_specular = vec3(1.0, 1.0, 1.0);
	vec3 light_diffuse = vec3(1.0, 1.0, 1.0);
	float diff = 0.4;
	float spec = 0.3;

	vec3 diffuse  = light_diffuse  * diff * texture(sdiffuse, frag_uv).rgb; 
	vec3 specular = light_specular * spec * texture(sspecular, frag_uv).rgb;
	vec3 bump = texture(sbump, frag_uv).rgb;
	frag_color = vec4(diffuse + specular + bump, 1.0);   
}
