#include "stdafx.h"
#include "light.h"
#include "shader.h"

int TakeLightType(LightType type)
{
	return glm::clamp(static_cast<int>(type) - 1,0,4) ;
}
void applyLightToShader(const LightSource& light, const ShaderProgram& shader, int channel)
{
	const std::string lightChannel = std::format("lights[{}].", channel);
	shader.glUniform(lightChannel + "position",light.position);
	shader.glUniform(lightChannel + "type", TakeLightType(light.type));
	shader.glUniform(lightChannel + "direction",light.direction);
	shader.glUniform(lightChannel + "cutOff",light.innerCutOff);
	shader.glUniform(lightChannel + "outerCutOff",light.outerCutOff);
	shader.glUniform(lightChannel + "constant",light.constant);
	shader.glUniform(lightChannel + "linear",light.linear);
	shader.glUniform(lightChannel + "quadratic",light.quadratic);
	shader.glUniform(lightChannel + "ambient",light.ambient);
	shader.glUniform(lightChannel + "diffuse",light.diffuse);
	shader.glUniform(lightChannel + "specular",light.specular);
}


