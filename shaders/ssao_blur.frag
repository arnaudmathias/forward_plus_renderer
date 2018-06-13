#version 410 core

in vec2 frag_uv;

out float frag_color;

uniform sampler2D ssao_tex;

void main() {
	vec2 texel_size = 1.0 / textureSize(ssao_tex, 0);
	float res = 0.0;
	for (int x = -2; x < 2; ++x) {
		for (int y = -2; y < 2; ++y) {
			vec2 offs = vec2(float(x), float(y)) *  texel_size;
			res += texture(ssao_tex, frag_uv + offs).r;
		}
	}
	frag_color = res / (4.0 * 4.0);
}

