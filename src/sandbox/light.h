#pragma once
#include "shader.h"



struct LightSourceBase
{
	glm::vec3 ambient;		// 环境光
	glm::vec3 diffuse;		// 漫反射光
	glm::vec3 specular;		// 镜面光
};

struct LightSourceDirection :public LightSourceBase
{
	glm::vec3 direction;
};

struct LightSourcePoint :public LightSourceBase
{
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct LightSourceSpot :public LightSourcePoint
{
	float innerCutOff;
	float outerCutOff;
	glm::vec3 direction;
};

struct LightSource {
	enum LightType
	{
		Direction,
		Point,
		Spot
	};
	LightSource(const LightSourceDirection& light);
	LightSource(const LightSourcePoint& light);
	LightSource(const LightSourceSpot& light);

	int type;
	float innerCutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;

	glm::vec3 direction;
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;


private:
	explicit LightSource(const LightSourceBase& light);
};

inline LightSource::LightSource(const LightSourceBase& light)
	: type(-1)
	, innerCutOff(0)
	, outerCutOff(0)
	, constant(0)
	, linear(0)
	, quadratic(0)
	, direction(), position()
	, ambient(light.ambient)
	, diffuse(light.diffuse)
	, specular(light.specular)

{
}

inline LightSource::LightSource(const LightSourceDirection& light)
	:LightSource(static_cast<LightSourceBase>(light))
{
	type = LightType::Direction;
	direction = light.direction;
}

inline LightSource::LightSource(const LightSourcePoint& light)
	:LightSource(static_cast<LightSourceBase>(light))
{
	type = LightType::Point;
	position = light.position;
	constant = light.constant;
	linear = light.linear;
	quadratic = light.quadratic;
}

inline LightSource::LightSource(const LightSourceSpot& light)
	: LightSource(static_cast<LightSourcePoint>(light))
{
	type = LightType::Spot;
	innerCutOff = light.innerCutOff;
	outerCutOff = light.outerCutOff;
	direction = light.direction;
}

void applyLightToShader(const LightSource& light, const ShaderProgram& shader, int channel = 0);
