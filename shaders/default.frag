#version 450 core

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
	vec3 position;
	float radius;
	vec3 color;
	float intensity;
};

struct LightingResult {
	vec3 diffuse;
	vec3 specular;
};

layout (std140, binding = 0) readonly buffer lights_data { 
	Light lights[];
};

layout(std430, binding = 1) readonly buffer visible_lights_indices {
	int lights_indices[];
};

layout (std140, binding = 2) uniform material_data { 
	Material material;
};

uniform int num_lights;
uniform int workgroup_x;
uniform int debug;
uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

vec3 getDiffuse(Light light, vec3 L, vec3 N) {
	float NdotL = max(dot(N, L), 0.0f);
	return light.color * NdotL;
}

vec3 getSpecular(Light light, vec3 V, vec3 L, vec3 N){
	vec3 R = normalize(reflect(-L, N));
	float RdotV = max(dot(R, V), 0);

	return light.color * pow(RdotV, material.specular_power);
	//return light.color * pow(RdotV, 4.0);
}

float getAttenuation(Light light, float d){
	return 1.0f - smoothstep(light.radius * 0.75f, light.radius, d);
}

/*LightingResult directionalLight(Light light, vec4 V, vec4 P, vec4 N) {
	LightingResult result;

	vec4 L = normalize(-light.direction_vs);

	result.diffuse = getDiffuse(light, L, N) * light.intensity;
	result.specular = getSpecular(light, V, L, N) * light.intensity;

	return result;
}*/

LightingResult pointLight(Light light, vec3 V, vec3 P, vec3 N) {
	LightingResult result;

	vec3 L = light.position - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = getAttenuation(light, distance);

	result.diffuse = getDiffuse(light, L, N) * attenuation * light.intensity;
	result.specular = getSpecular(light, V, L, N) * attenuation * light.intensity;

	return result;
}

LightingResult getLighting(vec3 eyePos, vec3 P, vec3 N) {
	vec3 V = normalize(eyePos - P);

	LightingResult totalResult;
	totalResult.diffuse = vec3(0.0f);
	totalResult.specular = vec3(0.0f);

	for (int i = 0; i < num_lights; i++){
		LightingResult result;
		//result = directionalLight(lights[i], V, P, N);
		result = pointLight(lights[i], V, P, N);
		totalResult.diffuse += result.diffuse;
		totalResult.specular += result.specular;
	}
	return totalResult;
}

void main() {
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint count = 0;
	uint index = tileID.y * workgroup_x + tileID.x;

	uint offset = index * 32;
	for (uint i = 0; i < 32; i++) {
		if (lights_indices[offset + i] != 0) {
			count++;
		}
	}

	vec4 diffuse = material.diffuse_color;
	if (material.has_diffuse_texture != 0){
		vec4 diffuse_color_tex = texture(diffuse_tex, frag_uv);
		diffuse = diffuse_color_tex;
	}

	float alpha = diffuse.a;

	LightingResult result;
	result = getLighting(vec3(0.0f, 0.0f, 0.0f), vs_pos, normalize(vs_normal)); 

	vec4 specular = vec4(0.0);
	if (material.specular_power > 1.0) {
		specular = material.specular_color;
		if (material.has_specular_texture != 0) {
			vec4 specular_color_tex = texture(specular_tex, frag_uv);
			specular = specular_color_tex;
		}
		specular = vec4(specular.rgb * result.specular, 1.0f);
	}
	
	if (debug == 0) {
		frag_color = vec4(specular.rgb + diffuse.rgb, 1.0);

	} else {
		float color = float(count) / 32.0;
		frag_color = vec4(color, color, color, 1.0f);
	}
}
