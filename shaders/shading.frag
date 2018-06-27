#version 450 core
#define MAX_LIGHTS_PER_TILE 1024
layout (location = 0) out vec4 out_hdr;
layout (location = 1) out vec3 out_normal;
const float PI = 3.14159265359;

in VS_OUT {
    vec2 frag_uv;
    mat3 TBN;
    vec3 ts_frag_pos;
    vec3 ts_view_pos;
} vs_in; 

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

#if __VERSION__ >= 430
layout (std140, binding = 0) readonly buffer lights_data { 
    Light lights[];
};

layout(std430, binding = 1) readonly buffer visible_lights_indices {
    int lights_indices[];
};

layout (std140, binding = 2) uniform material_data { 
    Material material;
};
#else
layout (std140) uniform lights_data { 
    Light lights[MAX_LIGHTS_PER_TILE];
};

layout (std140) uniform material_data { 
    Material material;
};
#endif

uniform mat4 M;
uniform int num_lights;
uniform int workgroup_x;
uniform int debug;

uniform sampler2DArray albedo_array;
uniform sampler2DArray normal_array;
uniform sampler2DArray metallic_array;
uniform sampler2DArray roughness_array;

uniform int albedo_tex;
uniform int normal_tex;
uniform int metallic_tex;
uniform int roughness_tex;

float get_attenuation(float light_radius, float dist) {

    float cutoff = 0.3;
    float denom = dist / light_radius + 1.0;
    float attenuation = 1.0 / (denom * denom);

    attenuation = (attenuation - cutoff) / (1 - cutoff);
    attenuation = max(attenuation, 0.0);
    return (attenuation);
}

vec3 fresnel_schlick(float cos_theta, vec3 f0) {
    return (f0 + (1.0 - f0) * pow(1.0 - cos_theta, 5.0));
} 

float distribution_ggx(vec3 normal, vec3 halfway, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float ndoth  = max(dot(normal, halfway), 0.0);
    float ndoth2 = ndoth*ndoth;
	
    float num   = a2;
    float denom = (ndoth2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return (num / denom);
}

float geometry_schlick_ggx(float ndotv, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num   = ndotv;
    float denom = ndotv * (1.0 - k) + k;
	
    return (num / denom);
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float ndotv = max(dot(N, V), 0.0);
    float ndotl = max(dot(N, L), 0.0);
    float ggx2  = geometry_schlick_ggx(ndotv, roughness);
    float ggx1  = geometry_schlick_ggx(ndotl, roughness);
    return (ggx1 * ggx2);
}

void main() {
    ivec2 loc = ivec2(gl_FragCoord.xy);
    ivec2 tileID = loc / ivec2(16, 16);
    uint index = tileID.y * workgroup_x + tileID.x;
    uint offset = index * MAX_LIGHTS_PER_TILE;

    vec3 ts_view_dir = normalize(vs_in.ts_view_pos - vs_in.ts_frag_pos);

    vec4 albedo4 = texture(albedo_array, vec3(vs_in.frag_uv, float(albedo_tex)));
    vec3 albedo = pow(albedo4.rgb, vec3(2.2));
    float alpha = albedo4.a;

    float metallic = texture(metallic_array, vec3(vs_in.frag_uv, float(metallic_tex))).r;
    float roughness = texture(roughness_array, vec3(vs_in.frag_uv, float(roughness_tex))).r;

    vec3 normal = texture(normal_array, vec3(vs_in.frag_uv, float(normal_tex))).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 f0 = vec3(0.04); 
    f0 = mix(f0, albedo, metallic);

    vec3 lo = vec3(0.0);
#if __VERSION__ >= 430
    for (uint i = 0; i < MAX_LIGHTS_PER_TILE; i++) {
	if (lights_indices[offset + i] != -1) {
	    int indices = lights_indices[offset + i];
#else
    for (uint i = 0; i < num_lights; i++) {
	    int indices = int(i);
#endif
	    vec3 ts_light_pos = vs_in.TBN * lights[indices].position;
	    vec3 ts_light_dir = normalize(ts_light_pos - vs_in.ts_frag_pos);
	    vec3 ts_halfway = normalize(ts_view_dir + ts_light_dir);

	    float dist = length(ts_light_pos - vs_in.ts_frag_pos);
	    float attenuation = get_attenuation(lights[indices].radius, dist);
	    vec3 radiance = lights[indices].color.rgb * attenuation;
	    float ndf = distribution_ggx(normal, ts_halfway, roughness);       
	    float g = geometry_smith(normal, ts_view_dir, ts_light_dir, roughness); 
	    vec3 f = fresnel_schlick(max(dot(ts_halfway, ts_view_dir), 0.0f), f0);

	    vec3 ks = f;
	    vec3 kd = vec3(1.0) - ks;
	    kd *= 1.0 - metallic;

	    vec3 numerator = ndf * g * f;
	    float denominator = 4.0 * max(dot(normal, ts_view_dir), 0.0) * max(dot(normal, ts_light_dir), 0.0);
	    vec3 specular = numerator / max(denominator, 0.001);

	    float ndotl = max(dot(normal, ts_light_dir), 0.0);                
	    lo += (kd * albedo / PI + specular) * radiance * ndotl; 
#if __VERSION__ >= 430
	}
#endif
    }
    vec3 ambient = vec3(0.03) * albedo;
    vec3 color = ambient + lo;
#if __VERSION__ >= 430
    if (debug == 1) {
	uint count;
	for (uint i = 0; i < MAX_LIGHTS_PER_TILE; i++) {
	    if (lights_indices[offset + i] != -1) {
		count++;
	    }
	}
	float shade = float(count) / float(MAX_LIGHTS_PER_TILE * 2); 
	color = vec3(shade);
    }
#endif
    out_hdr = vec4(color, alpha);
    out_normal = normal;
}
