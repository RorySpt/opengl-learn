#include "actor_light_component.h"

#include <imgui.h>

#include "actor.h"
#include "DebugDraw.h"
#include "player_controller.h"


void LightComponent::OnConstruct()
{
	PrimitiveComponent::OnConstruct();
	light_handle = GetOwner()->GetWorld()->_lights[channel].emplace_back(std::make_shared<LightSource>(LightSourceBase()));
}

void LightComponent::BeginPlay()
{
	PrimitiveComponent::BeginPlay();

	
}

void LightComponent::EndPlay()
{
	PrimitiveComponent::EndPlay();
	auto &lights = GetOwner()->GetWorld()->_lights[channel];
	const auto light = light_handle.lock();
	std::erase(lights, light);
}

void LightComponent::TickComponent(float deltaTime)
{
	PrimitiveComponent::TickComponent(deltaTime);

	
}

void LightComponent::draw(const Camera& camera)
{
}

void LightComponent::UI_Draw()
{
	ImGui::NewLine();
	ImGui::SeparatorText("Light Property");
	if (ImGui::ColorEdit3("LightColor", reinterpret_cast<float(&)[3]>(lightColor))){}
	ImGui::DragFloat3("LightRatio", &lightRatio[0], 0.1f, 0, 1);

	
	
}

std::weak_ptr<LightSource> LightComponent::GetLight()
{
	return light_handle;
}

void PointLightComponent::OnConstruct()
{
	LightComponent::OnConstruct();

}

void PointLightComponent::TickComponent(float deltaTime)
{
	LightComponent::TickComponent(deltaTime);

	auto light = GetLight().lock();
	*light = LightSourcePoint{
		{lightColor * lightRatio.x,lightColor * lightRatio.y,lightColor * lightRatio.z},GetWorldLocation()
		,constant,linear,quadratic
	};
}

void PointLightComponent::draw(const Camera& camera)
{
	LightComponent::draw(camera);


	if (b_show_model)
	{
		model.setLightColor({ lightColor});
		model.draw(camera, GetComponentToWorld());
	}
	
}

void PointLightComponent::UI_Draw()
{
	LightComponent::UI_Draw();
	ImGui::SliderFloat("Constant", &constant, 0, 1);
	ImGui::SliderFloat("Linear", &linear, 0, 1);
	ImGui::SliderFloat("Quadratic", &quadratic, 0, 1);
	ImGui::Checkbox("ShowModel", &b_show_model);
}

void DirectionLightComponent::TickComponent(float deltaTime)
{
	LightComponent::TickComponent(deltaTime);

	auto light = GetLight().lock();
	*light = LightSourceDirection{
		lightColor * lightRatio.x,lightColor * lightRatio.y,lightColor * lightRatio.z, GetWorldRotation() * glm::vec3{0,-1,0}
	};
}

void DirectionLightComponent::draw(const Camera& camera)
{
	LightComponent::draw(camera);
}

void DirectionLightComponent::UI_Draw()
{
	LightComponent::UI_Draw();
	if (const auto camera = GetOwner()->GetWorld()->GetPlayerController()->GetCameraManager()->ActivatedCamera().lock())
	{
		g_debug_draw->DrawLine(*camera,
			{ GetWorldLocation(),
				GetWorldLocation() + glm::vec3{GetComponentToWorld() * glm::vec4{glm::vec3{0,-10,0},1} } },
			{ 1,1,0,1 });
	}
}

void SpotLightComponent::TickComponent(float deltaTime)
{
	LightComponent::TickComponent(deltaTime);
	auto light = GetLight().lock();
	*light = LightSourceSpot{
		lightColor* lightRatio.x,lightColor* lightRatio.y,lightColor* lightRatio.z,GetWorldLocation()
		,constant,linear,quadratic
		,  glm::cos(glm::radians(innerCutOff)),  glm::cos(glm::radians(outerCutOff))
		, GetWorldRotation()* glm::vec3{0,0,-1}
	};
}

void SpotLightComponent::draw(const Camera& camera)
{
	LightComponent::draw(camera);


	if (b_show_model)
	{
		model.setLightColor(lightColor);
		model.draw(camera, GetComponentToWorld());
	}

	
}
void SpotLightComponent::UI_Draw()
{
	LightComponent::UI_Draw();
	ImGui::SliderFloat("Constant", &constant, 0, 1);
	ImGui::SliderFloat("Linear", &linear, 0, 1);
	ImGui::SliderFloat("Quadratic", &quadratic, 0, 1);
	if (ImGui::SliderFloat("InnerCutOff", &innerCutOff, 0, 180)) {


	}
	if(ImGui::SliderFloat("OuterCutOff", &outerCutOff, 0, 180))
	{
		if (outerCutOff < innerCutOff)outerCutOff = innerCutOff;
	}
	ImGui::Checkbox("ShowModel", &b_show_model);

	if (const auto camera = GetOwner()->GetWorld()->GetPlayerController()->GetCameraManager()->ActivatedCamera().lock())
	{
		g_debug_draw->DrawLine(*camera,
			{ GetWorldLocation(),
				glm::vec3{GetComponentToWorld() * glm::vec4{glm::vec3{0,0,-10},1} } },
			{ 1,1,0,1 });
	}
}