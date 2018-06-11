#version 410 core

in VS_OUT {
    vec2 frag_uv;
    mat3 TBN;
    vec3 ts_frag_pos;
    vec3 ts_view_pos;
} vs_in; 

out vec4 frag_color;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 transmittance;
    vec4 emission;

    float specular_power;
    float index_of_refraction;
    float opacity;

    float roughness;
    float metallic;
    float sheen;
    float clearcoat_thickness;
    float clearcoat_roughness;
    float anisotropy;
    float anisotropy_rotation;

    vec2 padding;
};

struct Light {
    vec3 position;
    float radius;
    vec3 color;
    float intensity;
};

struct LightingResult {
    vec3 diffuse;
    vec3 specular;
};

layout (std140) uniform light_data {
    Light lights[3];
};

layout (std140) uniform material_data { 
    Material material;
};

uniform mat4 M;
uniform int num_lights;
uniform int workgroup_x;
uniform int debug;

uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;
uniform sampler2D bump_tex;

float get_diffuse(vec3 light_dir, vec3 normal) {
    float diff = max(dot(light_dir, normal), 0.0);
    return (diff);
}

float get_specular(vec3 light_dir, vec3 view_dir, vec3 normal) {
    vec3 reflect_dir = reflect(-light_dir, normal);
    vec3 halfway_dir = normalize(light_dir + view_dir);  
    float spec = pow(max(dot(normal, halfway_dir), 0.0), material.specular_power);
    return (spec);
}

float get_attenuation(float light_radius, float dist) {

    float cutoff = 0.3;
    float denom = dist / light_radius + 1.0;
    float attenuation = 1.0 / (denom * denom);

    attenuation = (attenuation - cutoff) / (1 - cutoff);
    attenuation = max(attenuation, 0.0);
    return (attenuation);
}

void main() {
    ivec2 loc = ivec2(gl_FragCoord.xy);
    ivec2 tileID = loc / ivec2(16, 16);
    uint index = tileID.y * workgroup_x + tileID.x;

    vec4 diffuse = material.diffuse;
    vec4 diffuse_color_tex = texture(diffuse_tex, vs_in.frag_uv);
    diffuse = diffuse_color_tex;
    vec3 ambient = diffuse.rgb * 0.2f;

    float alpha = diffuse.a;

    vec3 normal = texture(bump_tex, vs_in.frag_uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 ts_view_dir = normalize(vs_in.ts_view_pos - vs_in.ts_frag_pos);

    vec4 out_color = vec4(ambient, 1.0);

    for (uint i = 0; i < num_lights; i++) {
	vec3 ts_light_pos = vs_in.TBN * lights[i].position;
	vec3 ts_light_dir = normalize(ts_light_pos - vs_in.ts_frag_pos);
	float dist = length(ts_light_pos - vs_in.ts_frag_pos);

	float attenuation = get_attenuation(lights[i].radius, dist);
	float diff = get_diffuse(ts_light_dir, normal) * attenuation;
	float specular = get_specular(ts_light_dir, ts_view_dir, normal) * attenuation;

	out_color += vec4((diff * diffuse.rgb) + (lights[i].color.rgb * specular), 0.0);
    }

    if (alpha < 0.3) {
	discard;
    }
    frag_color = out_color;
}
