#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

struct Light{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

//uniform vec3 objectColor;

uniform vec3 viewPos;
void main()
{
	vec3 diffuse = vec3(0),specular = vec3(0);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	vec3 viewDir = normalize(viewPos - FragPos);
	float lightDir_cos = dot(norm,lightDir);
	float viewDir_cos = dot(norm,viewDir);
	
	//环境光
	vec3 ambient = material.ambient*light.ambient;
	//漫反射
	//
	float diff = max(lightDir_cos,0.0);
	diffuse = diff*material.diffuse*light.diffuse;

	//镜面反射
	//发生的前提是光源和观察者在物体表面同侧
	if(lightDir_cos*viewDir_cos>0){
		vec3 reflectDir = reflect(-lightDir,norm);
		float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
		specular = spec * material.specular * light.specular;
	}
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result,1.0);
}