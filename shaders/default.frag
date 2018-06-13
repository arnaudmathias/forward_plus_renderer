#version 410 core

in vec2 frag_uv;

out vec4 frag_color;

uniform sampler2D hdr_tex;
uniform sampler2D ssao_tex;

void main() {
  float ao = texture(ssao_tex, frag_uv).r;
  vec3 color = texture(hdr_tex, frag_uv).rgb;
	color *= ao;
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0 / 2.2));  
  frag_color = vec4(color, 1.0f);	
}

