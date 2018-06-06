#version 410 core

in vec2 frag_uv;

out vec4 out_color;

uniform sampler2D sampler_text;
uniform vec3 color;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler_text, frag_uv).r);
    out_color = vec4(color, 1.0) * sampled;
}
