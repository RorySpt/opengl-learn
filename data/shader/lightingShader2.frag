#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material{
	//vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
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
	vec3 ambient = light.ambient * vec3(texture(material.diffuse,TexCoords));
	//漫反射
	//
	float diff = max(lightDir_cos,0.0);
	diffuse = diff*light.diffuse*vec3(texture(material.diffuse,TexCoords));

	//镜面反射
	//发生的前提是光源和观察者在物体表面同侧
	if(lightDir_cos*viewDir_cos>0){
		vec3 reflectDir = reflect(-lightDir,norm);
		float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
		specular = spec * vec3(texture(material.specular,TexCoords)) * light.specular;
	}
	//自发光
	vec3 v = vec3(texture(material.specular,TexCoords));
	v[0] = v[0] <= 0.01?1:0;
	v[1] = v[1] <= 0.01?1:0;
	v[2] = v[2] <= 0.01?1:0;
	vec3 emission = vec3(texture(material.emission,TexCoords))*v;

	vec3 result = ambient + diffuse + specular + emission;
	FragColor = vec4(result,1.0);
}