#pragma once

#include <imgui.h>

#include "actor.h"
#include "actor_light_component.h"
#include "actor_primitive_component.h"
#include "BoxModel.h"
#include "Model.h"
#include "camera.h"
#include "SnowFlakeModel.h"
#include "SphereModel.h"
#include "texture_loader.h"
#include "SpaceBox.h"


class SpaceBoxPrimitiveComponent :public PrimitiveComponent
{
public:
	SpaceBoxPrimitiveComponent()
	{
		model = std::make_shared<SpaceBox>();
	}
	void BeginPlay() override
	{
		PrimitiveComponent::BeginPlay();
		model->init();
	}
	void draw(const Camera& camera) override
	{
		model->draw(camera, GetComponentToWorld());
	}

	std::shared_ptr<SpaceBox> model;
};

class F22PrimitiveComponent :public PrimitiveComponent
{
public:
	void BeginPlay() override
	{
		bHasBlend = true;
		PrimitiveComponent::BeginPlay();

		shader = ShaderProgram::makeShaderByName("common.frag", "common.vert");
		model = std::make_shared<Model>(R"(C:\Users\zhang\Pictures\Material\F22\f22.obj)");
	}

	void draw(const Camera& camera) override
	{
		auto lights = GetOwner()->GetWorld()->GetLightsByChannel();
		if (lights.empty())return;

		shader->use();

		int i = 0;
		for (auto& light : lights)
		{
			applyLightToShader(light, *shader, i++);
		}


		shader->glUniform("viewPos", camera.Position);
		shader->glUniform("material.shininess", 32.0f);
		shader->glUniform("view", camera.getViewMatrix());
		shader->glUniform("projection", camera.getProjMatrix());

		shader->glUniform("model", GetComponentToWorld());
		model->Draw(*shader);
	}

	std::shared_ptr<ShaderProgram> shader;
	std::shared_ptr<Model> model;
};


class BoxPrimitiveComponent :public PrimitiveComponent
{
public:
	void draw(const Camera& camera) override
	{
		auto lights = GetOwner()->GetWorld()->GetLightsByChannel();
		if (lights.empty())
			model->setLight({});
		else
			model->setLight(lights);

		model->setMaterial(_material);
		model->_emission_ratio = _emission_ratio;
		model->draw(camera, GetComponentToWorld());
	}
	Material3 _material;
	inline static float _emission_ratio = 1.0f;
	std::shared_ptr<BoxModel_SimpleTexture> model = comm::getOrCreate<BoxModel_SimpleTexture>();
};


class SpherePrimitiveComponent :public PrimitiveComponent
{
public:
	SpherePrimitiveComponent()
	{
		auto material_names = MaterialTable::instance()->getMaterialNames();

		if (const auto res = std::ranges::find(material_names, "default"); res != material_names.end())
		{
			index = std::ranges::find(material_names, "default") - material_names.begin();
		}
		else
		{
			index = 0;
		}
		auto m = MaterialTable::instance()->getMaterial(material_names[index]);
		material.diffuse = m.diffuse;
		material.specular = m.specular;
		material.shininess = m.shininess;
		material.emission = glm::vec3{ 0,0,0 };

		model->initSphereVertexes(1, segmentation);
		model->UpdateBufferData();
	}


	void draw(const Camera& camera) override
	{
		model->_lights = GetOwner()->GetWorld()->GetLightsByChannel();
		model->_material = material;
		model->draw(camera, GetComponentToWorld());
	}
	void UI_Draw()
	{
		static auto ansi_names = MaterialTable::instance()->getMaterialNames();
		static std::vector<const char8_t*> utf8_names;
		static std::vector<std::unique_ptr<char8_t[]>> utf8_names_buffer;
		if (utf8_names.empty())
		{
			for (auto& name : ansi_names)
			{
				auto u8s = comm::AnsiToUtf8(name.c_str());

				utf8_names_buffer.emplace_back(std::make_unique<char8_t[]>(u8s.size() + 1));
				memcpy(utf8_names_buffer.back().get(), u8s.c_str(), u8s.size() + 1);

				utf8_names.emplace_back(utf8_names_buffer.back().get());

				//comm::println(comm::Utf8ToAnsi(utf8_names.back()));
			}
		}

		if (ImGui::Combo("Material", &index, reinterpret_cast<const char**>(utf8_names.data()), utf8_names.size()))
		{
			auto m = MaterialTable::instance()->getMaterial(ansi_names[index]);

			material.diffuse = m.diffuse;
			material.specular = m.specular;
			material.shininess = m.shininess;
		}

		if (ImGui::SliderInt("Segmentation", &segmentation, 3, 100))
		{
			model->initSphereVertexes(1, segmentation);
			model->UpdateBufferData();
		}

	}

private:
	Material3 material;
	int index;
	int segmentation = 20;
	std::shared_ptr<SphereModel> model = comm::getOrCreate<SphereModel>();
};
class LightActor :public Actor
{
public:
	LightActor()
	{
		_point_light_component = CreateDefaultComponent<PointLightComponent>();
		_point_light_component->AttachToComponent(_root_component);
		_point_light_component->SetRelativeScale3d(glm::vec3(0.2));

		_spot_light_component = CreateDefaultComponent<SpotLightComponent>();
		_spot_light_component->AttachToComponent(_root_component);
		_spot_light_component->SetRelativeScale3d(glm::vec3(0.2));
		_spot_light_component->lightColor = glm::vec3{ 0 };
	}

private:
	PointLightComponent* _point_light_component;
	SpotLightComponent* _spot_light_component;
};
class SpaceBoxActor : public Actor
{
public:
	SpaceBoxActor()
	{
		space_box_primitive = CreateDefaultComponent<SpaceBoxPrimitiveComponent>();

		space_box_primitive->AttachToComponent(_root_component);
		space_box_primitive->SetWorldScale3d(glm::vec3{ 1,1,1 } * 5000.f);

		direction_light_component = CreateDefaultComponent<DirectionLightComponent>();
		direction_light_component->AttachToComponent(_root_component);
		direction_light_component->lightColor = { 1, 1, 1 };
		direction_light_component->lightRatio = { 0.1f, 0.5f, 0.7f };

		direction_light_component->SetWorldRotation(convertToQuaternion({
			glm::radians(61.600),glm::radians(-153.200),glm::radians(0.0f)
		}));
	}
	SpaceBoxPrimitiveComponent* space_box_primitive;
	DirectionLightComponent* direction_light_component;
};
class MyActor :public Actor
{
public:
	MyActor()
	{
		
		{
			auto f22_primitive = CreateDefaultComponent<F22PrimitiveComponent>();
			f22_primitives.emplace_back(f22_primitive);

			f22_primitive->AttachToComponent(_root_component);
			f22_primitive->SetWorldRotation(convertToQuaternion({
				glm::radians(0.0f),glm::radians(0.f),glm::radians(0.0f)
				}));
		}
		{
			auto sphere_primitive = CreateDefaultComponent<SpherePrimitiveComponent>();
			sphere_primitive->AttachToComponent(_root_component);
			sphere_primitives.emplace_back(sphere_primitive);

			sphere_primitive->SetRelativeLocation({ 0,10,0 });
		}
		{
			std::default_random_engine mt(0);

			auto [container2_diffuse, container2_specular] = reinterpret_cast<std::array<unsigned int, 2>&>(comm::loadTexture({
				std::string(comm::dir_picture) + "/container2.png",
				std::string(comm::dir_picture) + "/container2_specular.png"
				})[0]);

			std::vector<TextureLoader::outer_type> setters;
			for (int i = 0; i < 100; ++i)
			{
				glm::vec3 pos;
				const double scale = std::uniform_real_distribution<>(0.78, 5.0)(mt);
				std::uniform_real_distribution<> urd(10, 50);
				std::uniform_int_distribution<> uid(0, 1);// 中间清理出一块空地
				pos = { urd(mt) * (uid(mt) ? 1 : -1), (scale - 1) / 2, urd(mt) * (uid(mt) ? 1 : -1) };

				auto box_primitive = CreateDefaultComponent<BoxPrimitiveComponent>();
				box_primitive->AttachToComponent(_root_component);
				box_primitive->SetRelativeLocation(pos);
				box_primitive->SetRelativeScale3d(glm::vec3{ static_cast<float>(scale) });

				box_primitive->_material = { container2_diffuse,
				container2_diffuse
				,container2_specular
				,32.0f };



				setters.emplace_back([box_primitive](TextureLoader::texture_id_type id)
					{
						box_primitive->_material.emission = id;
						std::cout << std::format("{}"" 加载纹理完成，ID: {}\n", box_primitive->display_name(), id);
					});

				box_primitives.emplace_back(box_primitive);
			}

			GetRandomRabbitTexture(setters);
		}




	}

	void GetRandomRabbitTexture(std::vector<TextureLoader::outer_type> setters);
	std::vector<F22PrimitiveComponent*> f22_primitives;
	std::vector<BoxPrimitiveComponent*> box_primitives;
	std::vector<SpherePrimitiveComponent*> sphere_primitives;
	
};
class SnowFlakePrimitiveComponent :public PrimitiveComponent
{
public:
	SnowFlakePrimitiveComponent()
	{
		_snow_flake_model = std::make_shared<SnowFlakeModel>();
	}
	void BeginPlay() override
	{
		PrimitiveComponent::BeginPlay();
		_snow_flake_model->init();
	}
	void draw(const Camera& camera) override
	{
		_snow_flake_model->draw(camera, GetComponentToWorld());
	}
	void UI_Draw() override
	{
		ImGui::ColorEdit4("Color", &_snow_flake_model->color.x);
		float LINE_WIDTH_RANGE[2];
		glGetFloatv(GL_LINE_WIDTH_RANGE, LINE_WIDTH_RANGE);
		ImGui::SliderFloat("Line Width", &_snow_flake_model->line_width, 0, LINE_WIDTH_RANGE[1]);
		//GLboolean smooth;
		//glGetBooleanv(GL_LINE_SMOOTH, &smooth);
		//if(bool b = smooth == GL_FALSE ? false : true; ImGui::Checkbox("Line Smooth", &b))
		//{
		//	if (b)glEnable(GL_LINE_SMOOTH);
		//	else glDisable(GL_LINE_SMOOTH);
		//}
		int times = _snow_flake_model->iterations;
		if (ImGui::SliderInt("Iteration Times", &times, 0, 14))
		{
			_snow_flake_model->iterations = times;
		}
		ImGui::SeparatorText("Iterations");
		for (int i = 0; i < _snow_flake_model->vertexes.size(); ++i)
		{
			auto& vs = _snow_flake_model->vertexes[i];
			ImGui::Text(std::format("Iteration[{}] vertex num is {}", i, vs.size()).c_str());
		}
	}
private:
	std::shared_ptr<SnowFlakeModel> _snow_flake_model;
};
class SnowFlakeActor : public Actor
{
public:
	SnowFlakeActor()
	{
		space_box_primitive = CreateDefaultComponent<SnowFlakePrimitiveComponent>();
		space_box_primitive->AttachToComponent(_root_component);
	}

private:
	SnowFlakePrimitiveComponent* space_box_primitive;
};
