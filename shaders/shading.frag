#version 410 core
const float PI = 3.14159265359;
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
    Light lights[1];
};

layout (std140) uniform material_data { 
    Material material;
};

uniform mat4 M;
uniform int num_lights;
uniform int workgroup_x;
uniform int debug;

uniform sampler2D albedo_tex;
uniform sampler2D metallic_tex;
uniform sampler2D roughness_tex;
uniform sampler2D normal_tex;

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

    vec3 ts_view_dir = normalize(vs_in.ts_view_pos - vs_in.ts_frag_pos);

    vec4 albedo4 = texture(albedo_tex, vs_in.frag_uv);
    vec3 albedo = pow(albedo4.rgb, vec3(2.2));
    float alpha = albedo4.a;

    float metallic = texture(metallic_tex, vs_in.frag_uv).r;
    float roughness = texture(roughness_tex, vs_in.frag_uv).r;

    vec3 normal = texture(normal_tex, vs_in.frag_uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 f0 = vec3(0.04); 
    f0 = mix(f0, albedo, metallic);

    vec3 lo = vec3(0.0);
    for (uint i = 0; i < num_lights; i++) {
	vec3 ts_light_pos = vs_in.TBN * lights[i].position;
	vec3 ts_light_dir = normalize(ts_light_pos - vs_in.ts_frag_pos);
	vec3 ts_halfway = normalize(ts_view_dir + ts_light_dir);

	float dist = length(ts_light_pos - vs_in.ts_frag_pos);
	float attenuation = get_attenuation(lights[i].radius, dist);
	//float attenuation = 1.0 / (dist * dist);	
	
	vec3 radiance = lights[i].color.rgb * attenuation;
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
    }
    vec3 ambient = vec3(0.03) * albedo;
    vec3 color = ambient + lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));  

    if (alpha < 0.3) {
	discard;
    }
    frag_color = vec4(color, 1.0);
}
