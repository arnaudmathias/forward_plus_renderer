#version 410 core

in vec2 frag_uv;

out vec4 out_color;

uniform sampler2D sampler_ui;
uniform vec3 text_color;

void main() {
    vec4 sampled = texture(sampler_ui, frag_uv);
    out_color = sampled;
}
