#version 450 core
#define NUM_LIGHTS 10
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2 

in vec3 vs_pos;
in vec3 vs_normal;
in vec2 frag_uv;

out vec4 frag_color;

struct Material {
	vec4 global_ambient;
	vec4 ambient_color;
	vec4 emissive_color;
	vec4 diffuse_color;
	vec4 specular_color;
	vec4 reflectance;
	float opacity;
	float specular_power;
	float index_of_refraction;
	uint has_ambient_texture;
	uint has_emissive_texture;
	uint has_diffuse_texture;
	uint has_specular_texture;
	uint has_specular_power_texture;
	uint has_normal_texture;
	uint has_bump_texture;
	uint has_opacity_texture;
	float bump_intensity;
	float specular_Scale;
	float alpha_threshold;
	vec2 padding;
};

struct Light {
	vec4 position_ws;
	vec4 direction_ws;
	vec4 position_vs;
	vec4 direction_vs;
	vec4 color;
	float spotlight_angle;
	float range;
	float intensity;
	uint enabled;
	uint selected;
	uint type;
	vec2 padding;
};

struct LightingResult {
	vec4 diffuse;
	vec4 specular;
};

layout (std140, binding = 0) uniform data { 
	Light lights[NUM_LIGHTS];
	Material material;
};

uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

vec4 getDiffuse(Light light, vec4 L, vec4 N) {
	float NdotL = max(dot(N, L), 0.0f);
	return light.color * NdotL;
}

vec4 getSpecular(Light light, vec4 V, vec4 L, vec4 N){
	vec4 R = normalize(reflect(-L, N));
	float RdotV = max(dot(R, V), 0);

	return light.color * pow(RdotV, material.specular_power);
	//return light.color * pow(RdotV, 4.0);
}

float getAttenuation(Light light, float d){
	return 1.0f - smoothstep(light.range * 0.75f, light.range, d);
}

LightingResult directionalLight(Light light, vec4 V, vec4 P, vec4 N) {
	LightingResult result;

	vec4 L = normalize(-light.direction_vs);

	result.diffuse = getDiffuse(light, L, N) * light.intensity;
	result.specular = getSpecular(light, V, L, N) * light.intensity;

	return result;
}

LightingResult pointLight(Light light, vec4 V, vec4 P, vec4 N) {
	LightingResult result;

	vec4 L = light.position_vs - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = getAttenuation(light, distance);

	result.diffuse = getDiffuse(light, L, N) * attenuation * light.intensity;
	result.specular = getSpecular(light, V, L, N) * attenuation * light.intensity;

	return result;
}

LightingResult getLighting(vec4 eyePos, vec4 P, vec4 N) {
	vec4 V = normalize(eyePos - P);

	LightingResult totalResult;
	totalResult.diffuse = vec4(0.0f);
	totalResult.specular = vec4(0.0f);

	for (int i = 0; i < 10; i++){
		LightingResult result;

		if (lights[i].enabled == 0)
			continue;
		if (lights[i].type != DIRECTIONAL_LIGHT &&
				length(lights[i].position_vs - P) > lights[i].range) continue;

		switch (lights[i].type) {
			case DIRECTIONAL_LIGHT:
				{
					result = directionalLight(lights[i], V, P, N);
				}
				break;
			case POINT_LIGHT:
				{
					result = pointLight(lights[i], V, P, N);
				}
				break;
		}
		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}
	return totalResult;
}

void main() {
	vec4 diffuse = material.diffuse_color;
	if (material.has_diffuse_texture != 0){
		vec4 diffuse_color_tex = texture(diffuse_tex, frag_uv);
		diffuse = diffuse_color_tex;
	}

	float alpha = diffuse.a;

	LightingResult result;
	result = getLighting(vec4(0.0f, 0.0f, 0.0f, 1.0f), vec4(vs_pos, 1.0f), normalize(vec4(vs_normal, 0.0f))); 

	vec4 specular = vec4(0.0);
	if (material.specular_power > 1.0) {
		specular = material.specular_color;
		if (material.has_specular_texture != 0) {
			vec4 specular_color_tex = texture(specular_tex, frag_uv);
			specular = specular_color_tex;
		}
		specular *= result.specular;
	}

	frag_color = vec4(specular.rgb + diffuse.rgb, 1.0);
}
