#version 330 core

out			vec4 FragColor;
in			vec3 Normal;
in			vec3 FragPos;
in			vec2 TexCoords;
uniform		vec3 viewPos;

struct Material{
	bool enable_diffuseTex;
	bool enable_specularTex;
	bool enable_emissionTex;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	sampler2D diffuseTex;
	sampler2D specularTex;
	sampler2D emissionTex;
	float shininess;
};
uniform Material material;

struct MaterialVal{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
};


struct Light{
	vec3  position;

	int   type;
	vec3  direction;
	float innerCutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
};
#define NR_LIGHTS 16
uniform Light lights[NR_LIGHTS];

#define LT_DIRECT 0
#define LT_POINT  1
#define LT_SPOT   2

vec3 CalcLight(MaterialVal mv, Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// 属性
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	MaterialVal materialVal;
	materialVal.ambient  = material.enable_diffuseTex ?vec3(texture(material.diffuseTex,  TexCoords)):material.diffuse;
	materialVal.diffuse  = material.enable_diffuseTex ?vec3(texture(material.diffuseTex,  TexCoords)):material.diffuse;
	materialVal.specular = material.enable_specularTex?vec3(texture(material.specularTex, TexCoords)):material.specular;
	materialVal.emission = material.enable_emissionTex?vec3(texture(material.emissionTex, TexCoords)):material.emission;
	materialVal.shininess =  material.shininess;
	
	vec3 result;
	for(int i = 0; i < NR_LIGHTS; ++i){
		result += CalcLight(materialVal,lights[i], normal, FragPos, viewDir);
	}

	//自发光
	vec3 v = vec3(texture(material.specularTex,TexCoords));
	v[0] = v[0] <= 0.01?1:0;
	v[1] = v[1] <= 0.01?1:0;
	v[2] = v[2] <= 0.01?1:0;
	result += v * materialVal.emission;

	FragColor = vec4(result,1.0);
}

vec3 CalcLight(MaterialVal mv, Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = light.type == LT_DIRECT? normalize(-light.direction) : normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	if(light.type == LT_SPOT && theta < light.outerCutOff){
			return light.ambient * mv.ambient;
	}
	// 镜面反射色
	float diff = max(dot(normal,lightDir),0.0);
	// 镜面光着色
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);//material.shininess

	// 合并结果
	vec3 ambient = light.ambient * mv.ambient;
	vec3 diffuse = diff * light.diffuse * mv.diffuse;
	vec3 specular = spec * light.specular * mv.specular;
	
	float intensity = 1.0;
	float attenuation = 1.0;

	if(light.type == LT_SPOT){
		float epsilon = light.innerCutOff - light.outerCutOff;
		intensity = clamp((theta - light.outerCutOff)/epsilon,0.0,1.0);
	}
	if(light.type > 0){
		// 衰减
		float distance = length(light.position - fragPos);
		attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	}
	
	float ratio = intensity * attenuation;
	
	ambient *= ratio;
	diffuse *= ratio;
	specular *= ratio;

	return (ambient + diffuse + specular);
}