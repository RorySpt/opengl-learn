#pragma once
#include "actor_primitive_component.h"
#include "light.h"
#include "IlluminantModel.h"
class LightComponent : public PrimitiveComponent
{
public:
	virtual void OnConstruct() override;
	virtual void BeginPlay() override;
	virtual void EndPlay() override;
	virtual void TickComponent(float deltaTime) override;
	virtual void draw(const Camera& camera) override;

	virtual void UI_Draw() override;

	std::weak_ptr<LightSource> GetLight();

	glm::vec3 lightColor = { 1, 1, 1 };
	glm::vec3 lightRatio = { 0.3f, 0.7f, 1.0f }; //ambient, diffuse, specular
	;
	int channel = 0;									   
private:
	std::weak_ptr<LightSource> light_handle;
};


class PointLightComponent : public LightComponent
{
public:
	void OnConstruct() override;
	void TickComponent(float deltaTime) override;
	void draw(const Camera& camera) override;
	void UI_Draw() override;
	float constant = 1.f;
	float linear = 0;
	float quadratic = 0;

	bool b_show_model = true;
	IlluminantModel model;
};


class DirectionLightComponent : public LightComponent
{
public:

	void TickComponent(float deltaTime) override;
	void draw(const Camera& camera) override;
	void UI_Draw() override;
};

class SpotLightComponent : public LightComponent
{
public:

	void TickComponent(float deltaTime) override;
	void draw(const Camera& camera) override;
	void UI_Draw() override;
	float constant = 1.f;
	float linear = 0;
	float quadratic = 0;
	float innerCutOff = 12.5f;
	float outerCutOff = 17.5f;

	bool b_show_model = true;
	IlluminantModel model;
};
