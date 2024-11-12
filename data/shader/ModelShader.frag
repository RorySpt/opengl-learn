#version 330 core

out			vec4 FragColor;
in			vec3 Normal;
in			vec3 FragPos;
in			vec2 TexCoords;
uniform		vec3 viewPos;

struct Material{
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	sampler2D diffuseTex;
	sampler2D specularTex;
	sampler2D emissionTex;
	float shininess;
};
uniform Material material;

struct PureMaterial{
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
};


struct Light{
	vec3  position;

	int   type;
	vec3  direction;
	float cutOff;
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

vec3 CalcLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 viewDir);
vec3 CalcDirLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{
	// 属性
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
    PureMaterial pureMaterial;
	pureMaterial.diffuse  = vec3(texture(material.diffuseTex,  TexCoords)) + material.diffuse;
	pureMaterial.specular = vec3(texture(material.specularTex, TexCoords)) + material.specular;
	pureMaterial.emission = vec3(texture(material.emissionTex, TexCoords)) + material.emission;
	pureMaterial.shininess =  material.shininess;
	
	vec3 result;
	for(int i = 0; i < NR_LIGHTS; ++i){
		//switch(lights[i].type){
		//	case LT_DIRECT: result += CalcDirLight(lights[i], normal, viewDir); break;
		//	case LT_POINT: result += CalcPointLight(lights[i], normal, FragPos, viewDir);break;
		//	case LT_SPOT: result += CalcSpotLight(lights[i], normal, FragPos, viewDir);break;
		//	default : break;
		//}
		result += CalcLight(pureMaterial, lights[i], normal, viewDir);
	}

	//自发光
	//vec3 v = pureMaterial.specular;
	//v[0] = v[0] <= 0.01?1:0;
	//v[1] = v[1] <= 0.01?1:0;
	//v[2] = v[2] <= 0.01?1:0;
	//result += v * pureMaterial.emission;

	FragColor = vec4(result,1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), pureMaterial.shininess);
    // combine results
    vec3 ambient = light.ambient * pureMaterial.diffuse;
    vec3 diffuse = light.diffuse * diff * pureMaterial.diffuse;
    vec3 specular = light.specular * spec * pureMaterial.specular;
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), pureMaterial.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * pureMaterial.diffuse;
    vec3 diffuse = light.diffuse * diff * pureMaterial.diffuse;
    vec3 specular = light.specular * spec * pureMaterial.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), pureMaterial.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * pureMaterial.diffuse;
    vec3 diffuse = light.diffuse * diff * pureMaterial.diffuse;
    vec3 specular = light.specular * spec * pureMaterial.specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

vec3 CalcLight(PureMaterial pureMaterial, Light light, vec3 normal, vec3 viewDir)
{

	vec3 lightDir = light.type == LT_DIRECT  ? 
					  normalize(-light.direction) 
					: normalize(light.position - FragPos);
	
	float theta = dot(lightDir, normalize(-light.direction));
	
	if(light.type == LT_SPOT && theta < light.outerCutOff)
	{
			return vec3(0,0,0);
	}
	// 镜面反射色
	float diff = max(dot(normal,lightDir),0.0);
	// 镜面光着色
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),pureMaterial.shininess);//pureMaterial.shininess

	// 合并结果
	vec3 ambient = light.ambient * pureMaterial.diffuse;
	vec3 diffuse = diff * light.diffuse * pureMaterial.diffuse;
	vec3 specular = spec * light.specular * pureMaterial.specular;
	
	float intensity = 1.0;
	float attenuation = 1.0;

	if(light.type == LT_SPOT){
		float epsilon = light.cutOff - light.outerCutOff;
		intensity = clamp((theta - light.outerCutOff)/epsilon,0.0,1.0);
	}
	if(light.type > 0){
		// 衰减
		float distance = length(light.position - FragPos);
		attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	}
	
	float ratio = intensity * attenuation;
	
	ambient *= ratio;
	diffuse *= ratio;
	specular *= ratio;

	return (ambient + diffuse + specular);
}