#version 410 core

in vec2 frag_uv;

out vec4 frag_color;

uniform sampler2D hdr_tex;

const float exposure = 2.0f;
const float gamma = 2.2;

vec3 uncharted2_tonemap(vec3 x){
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 tone_mapping(vec3 x) {

	vec3 mapped = vec3(1.0) - exp(-x * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));
	return (mapped);
}

void main() {
	vec3 color = texture(hdr_tex, frag_uv).rgb;
	float W = 11.2;
	vec3 white_scale = 1.0f / uncharted2_tonemap(vec3(W));
	color *= 16.0;
	color = uncharted2_tonemap(color * exposure) * white_scale;
	color = pow(color, vec3(1.0 / gamma));
	frag_color = vec4(color, 1.0f);	
}
