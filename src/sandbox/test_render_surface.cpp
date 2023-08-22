#include "stdafx.h"
#include "test_render_surface.h"

#include <execution>
#include <stack>
#include <typeindex>

#include "actor_camera_component.h"
#include "actor_light_component.h"
#include "actor_primitive_component.h"
#include "camera_actor.h"
#include "Model.h"
#include "player_controller.h"
#include "texture_loader.h"





class F22PrimitiveComponent :public PrimitiveComponent
{
public:
	void BeginPlay() override
	{
		PrimitiveComponent::BeginPlay();

		shader = ShaderProgram::makeShaderByName("common.frag", "common.vert");
		model = std::make_shared<Model>(R"(C:\Users\zhang\Pictures\Material\F22\f22.obj)");
	}

	void draw(const Camera& camera) override
	{
		auto lights = GetOwner()->GetWorld()->GetLightsByChannel();
		if(lights.empty())return;
		
		shader->use();

		int i = 0;
		for(auto& light: lights)
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


class BoxPrimitiveComponent:public PrimitiveComponent
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

class SphereModel :IModel
{
public:
	struct vertex

	{
		
	};
	SphereModel()
	{
		glCreateVertexArrays(1, &VAO);
		glCreateBuffers(1, &VBO);
		
		//glNamedBufferData(VBO,)
		//glVertexArrayVertexBuffer(VAO,0,VBO,)


	}

	void draw(const Camera& camera, const glm::mat4& wMat) override {};

private:
	GLuint VBO;
	GLuint VAO;

	std::vector<>
};
class SpherePrimitiveComponent :public PrimitiveComponent
{
	
	SpherePrimitiveComponent()
	{
		
	}


	
};
class LightActor:public Actor
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

class MyActor:public Actor
{
public:
	MyActor()
	{
		{
			auto f22_primitive = CreateDefaultComponent<F22PrimitiveComponent>();
			f22_primitives.emplace_back(f22_primitive);
			
			f22_primitive->AttachToComponent(_root_component);

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
};
void MyActor::GetRandomRabbitTexture(std::vector<TextureLoader::outer_type> setters)
{
	namespace fs = std::filesystem;
	constexpr std::string_view RabbitDir = R"(C:\Users\zhang\Pictures\4K壁纸)";
	static std::vector<fs::path> rabbitPics;
	//static std::random_device rd;
	static std::default_random_engine dre(0);

	if (rabbitPics.empty())
	{
		for (auto& p : fs::directory_iterator(RabbitDir))
		{
			if (auto path = p.path(); path.has_extension())
				if (auto ext = path.extension(); ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp")
				{
					rabbitPics.emplace_back(p.path());
				}
		}
	}
	if (!rabbitPics.empty())
	{
		std::uniform_int_distribution uid(0, static_cast<int>(rabbitPics.size()) - 1);
		int index = uid(dre);

		std::vector<TextureLoader::input_type> inputs(setters.size());
		for(int i = 0;i < inputs.size();++i)
		{
			inputs[i] = TextureLoader::input_type{
				fs::path(rabbitPics[uid(dre)].c_str()),
				setters[i]
			};
		}
		g_texture_loader->loadTexture(inputs);
	}
	
}

template<typename ...Ty>
bool ScopeColor_TreeNode(ImVec4 color_f, Ty... args)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color_f);
	auto ret = ImGui::TreeNode(std::forward<Ty>(args)...);
	ImGui::PopStyleColor();
	return ret;
}

template<typename Ret, typename ...Ty>
Ret ScopeColor(ImVec4 color_f, Ret (*func)(Ty...), Ty... args)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color_f);
	Ret ret = func(std::forward<Ty>(args)...);
	ImGui::PopStyleColor();
	return ret;
}

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void TestRenderSurface::UI_Scene()
{
	ImGui::Begin("World");


	static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
	ImGui::ColorEdit4("ObjectNameColor", &colf.x);

	
	ImGui::SliderFloat("EmissionRatio", &BoxPrimitiveComponent::_emission_ratio, 0, 1);

	
	std::stack<std::string> level;
	level.emplace("World");
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);

	if (ImGui::CollapsingHeader(level.top().c_str()))
	{
		ImGuiIO& io = ImGui::GetIO();

		for (auto actor : world._workActors)
		{
			// Foreach Actor
			auto actor_type_name = std::type_index(typeid(*actor)).name();
			level.emplace(actor_type_name);
			if (ScopeColor_TreeNode(colf, std::format("{}##{}", actor->display_name(), level.size()).c_str()))
			{
				ImGui::SeparatorText("Property");
				ImGui::BulletText(std::format("TypeName: {}", actor_type_name).c_str());
				ImGui::BulletText(std::format("Name: {}", actor->name()).c_str());
				ImGui::BulletText("More details.");

				// CameraManager
				{
					if (const auto camera_manager = dynamic_cast<CameraManager*>(actor))
					{
						ImGui::BulletText(std::format("Camera Num: {}", camera_manager->cameras.size()).c_str());
					}
				}




				ImGui::SeparatorText(std::format("Components").c_str());
				for (auto& component : actor->_non_scene_components)
				{
					// Foreach _non_scene_components
					const auto com_type_name = std::type_index(typeid(*component)).name();
					level.push(com_type_name);
					if (ScopeColor_TreeNode(colf, std::format("{}##{}", component->display_name(), level.size()).c_str()))
					{
						ImGui::SeparatorText("Property");
						ImGui::BulletText(std::format("TypeName: {}", actor_type_name).c_str());
						ImGui::BulletText(std::format("Name: {}", component->name()).c_str());
						ImGui::BulletText("More details.");

						ImGui::TreePop();
						ImGui::Spacing();
					}
					level.pop();
				}
				int count = 0;
				std::function<void(SceneComponent*)> UI_SceneComponent = [&](SceneComponent* component)
				{
					// Foreach _scene_components
					int index = count++;

					auto com_type_name = std::type_index(typeid(*component)).name();
					level.push(com_type_name);
					if (ScopeColor_TreeNode(colf, std::format("{}##{}", index == 0 ? std::format("Root({})", component->display_name()) : component->display_name(), level.size()).c_str()))
					{
						ImGui::SeparatorText("Property");
						ImGui::BulletText(std::format("TypeName: {}", com_type_name).c_str());
						ImGui::BulletText(std::format("Name: {}", component->name()).c_str());


						ImGui::SameLine(0,100);
						static bool bRelative = true;
						if (ImGui::SmallButton(bRelative ? "Relative" : "World"))
						{
							bRelative = !bRelative;
						}

						std::string tr_type = bRelative ? "Relative-" : "World-";

						ImGui::SameLine(); HelpMarker("Display the relative transform parts or world transform parts.");
						auto pos = bRelative ? component->GetRelativeLocation() : component->GetWorldLocation();
						auto rot = glm::degrees(convertToEulerAngle(bRelative ? component->GetRelativeRotation() : component->GetWorldRotation()).data);
						auto sca = bRelative ? component->GetRelativeScale3d() : component->GetWorldScale3d();
						auto qua = bRelative ? component->GetRelativeRotation() : component->GetWorldRotation();

						//ImGui::BulletText(std::format("Relative Position: {:.3f}, {:.3f}, {:.3f}", pos[0], pos[1], pos[2]).c_str());
						//ImGui::BulletText(std::format("Relative Rotation: {:.3f}, {:.3f}, {:.3f}", rot.data[0], rot.data[1], rot.data[2]).c_str());
						//ImGui::BulletText(std::format("Relative Scale3d : {:.3f}, {:.3f}, {:.3f}", sca[0], sca[1], sca[2]).c_str());
						if(ImGui::DragFloat3((tr_type +"Position").c_str(), &pos[0], 0.1f))
						{
							if (bRelative)component->SetRelativeLocation(pos);
							else component->SetWorldLocation(pos);
						}
						ImGui::SameLine(); HelpMarker("opengl right-hand coordinate system, right as x, up as y, front as -z.");

						if (ImGui::DragFloat3((tr_type + "Rotation").c_str(), &rot[0], 0.1f))
						{
							rot[0] = glm::clamp(rot[0], -89.9f, 89.9f);
							if (bRelative)component->SetRelativeRotation(convertToQuaternion(glm::radians(rot)));
							else component->SetWorldRotation(convertToQuaternion(glm::radians(rot)));
						}
						ImGui::SameLine(); HelpMarker("pitch as x, yaw as y, roll as z.");
						if constexpr (0)
						{

							auto rot_ = glm::degrees(convertToEulerAngle_native(bRelative ? component->GetRelativeRotation() : component->GetWorldRotation()).data);
							if (ImGui::DragFloat3("Rotation_native", &rot_[0], 0.1f))
							{
								if (bRelative)component->SetRelativeRotation(convertToQuaternion_native(glm::radians(rot_)));
								else component->SetWorldRotation(convertToQuaternion_native(glm::radians(rot_)));
							}
							auto rot_to = glm::degrees(convertToQuaternion_(glm::radians(rot)).data);
							if (ImGui::DragFloat3("Rotation_to", &rot_to[0], 0.1f))
							{
							}
						}

						static bool relevance = false;
						auto scaTmp = sca;
						if (ImGui::DragFloat3((tr_type + "Scale3d").c_str(), &sca[0], 0.1f))
						{
							if(relevance)
							{
								float value;
								for(int n = 0; n < 3;++n)
								{
									if (sca[n] != scaTmp[n])value = sca[n];
								}
								sca = { value,value,value };
							}
							if (bRelative)component->SetRelativeScale3d(sca);
							else component->SetWorldScale3d(sca);
						}
						ImGui::SameLine();
						ImGui::Checkbox(" ", &relevance);
						if (ImGui::BeginItemTooltip())
						{
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::TextUnformatted("link the x, y, z.");
							ImGui::PopTextWrapPos();
							ImGui::EndTooltip();
						}
						if (ImGui::DragFloat4((tr_type + "Quaternion").c_str(), &qua[0], 0.1f))
						{
							if (bRelative)component->SetRelativeRotation(qua);
							else component->SetRelativeRotation(qua);
						}

						// CameraComponent
						{
							if (const auto camera_component = dynamic_cast<CameraComponent*>(component))
							{
								auto _loc_world = camera_component->GetComponentToWorld();
								ImGui::Text("%s", std::format("Camera Position: {:.3f}, {:.3f}, {:.3f}"
									, _loc_world[3][0], _loc_world[3][1], _loc_world[3][2]).c_str());
							}
						
						}
						// LightComponent
						{
							if (const auto camera_component = dynamic_cast<LightComponent*>(component))
							{
								camera_component->UI_Draw();
							}

						}


						if (component->GetNumChildrenComponents() > 0)
							ImGui::SeparatorText(std::format("SubComponents").c_str());
						for(const auto sub:component->GetAttachChildren())
						{
							UI_SceneComponent(sub);
						}
						ImGui::TreePop();
						ImGui::Spacing();
					}
					level.pop();
				};
				UI_SceneComponent(actor->_root_component);

				
				ImGui::TreePop();
				ImGui::Spacing();
			}
			level.pop();
		}
	}
	level.pop();

	ImGui::End();
}

void TestRenderSurface::InitWorld()
{
	world.init(_window);

	camera_manager = world.GetPlayerController()->GetCameraManager();
	input = world.GetPlayerController()->GetInputManager();
	input->EnableInput();
	world.SpawnActor<CameraActor>();

	auto mesh = world.SpawnActor<Actor>();
	mesh->set_name("MyMesh");
	mesh = world.SpawnActor<Actor>();
	mesh->set_name("MyMesh");

	world.SpawnActor<MyActor>();

	static glm::vec3 lightColor(1, 1, 1);
	static glm::vec3 lightRatio(0.3f, 0.7f, 1);
	

	
	glm::vec3 lightWorldPos = glm::vec4{ 1.2f, 5.2f, 10.0f, 1.0f } *2.0f;

	// 光源

	auto lightActor = world.SpawnActor<LightActor>();
	lightActor->_root_component->SetWorldLocation(lightWorldPos);

	//Light light = { lightColor * lightRatio.x,lightColor * lightRatio.y,lightColor * lightRatio.z, lightWorldPos };
	//
	//world._lights[0] = LightSourcePoint(light);
}

void TestRenderSurface::UI_CameraTest()
{
	ImGui::Begin("CameraTest");
	for (auto& actor : world._workActors)
	{
		for (auto& component : actor->_owned_components)
		{
			auto camera_component = dynamic_cast<CameraComponent*>(component.get());
			if (camera_component)
			{
				auto _loc_world = camera_component->GetComponentToWorld();
				static bool open = true;
				ImGui::Begin("CameraTest", &open);
				ImGui::Text("%s", std::format("Camera Position: {:.3f}, {:.3f}, {:.3f}"
					, _loc_world[3][0], _loc_world[3][1], _loc_world[3][2]).c_str());

				ImGui::End();
			}
		}
	}
	ImGui::End();
}




TestRenderSurface::~TestRenderSurface() = default;

TestRenderSurface::TestRenderSurface()
	:mt(std::random_device("")())
{
	
}

void TestRenderSurface::init(GLFWwindow* window)
{
	_window = window;
	std::cout << __func__ << "\n";

	texture_loader = std::make_shared<TextureLoader>(window);
	::g_texture_loader = texture_loader.get();

	InitWorld();

}

void TestRenderSurface::exit()
{
	std::cout << __func__ << "\n";
	world.EndPlay();
}

void TestRenderSurface::tick(float deltaTime)
{
	if (!world.bHasBegunPlay)
		world.BeginPlay();

	world.Tick(deltaTime);

	g_texture_loader->tick();
}

void TestRenderSurface::draw(float deltaTime)
{
	UI_Scene();
	const auto camera = world.GetPlayerController()->GetCameraManager()->ActivatedCamera().lock();
	for(auto actor:world._workActors)
	{
		for(auto&component:actor->_owned_components)
		{
			auto primitive = dynamic_cast<PrimitiveComponent*>(component.get());
			if(primitive)
			{
				primitive->draw(*camera);
			}
		}
	}
}

void TestRenderSurface::resizeEvent(int width, int height)
{
	GUIInterface::resizeEvent(width, height);
	camera_manager->ResizeViewport(width, height);
}

void TestRenderSurface::keyEvent(int keyCode, int scanCode, int keyAction, int keyModifiers)
{
	GUIInterface::keyEvent(keyCode, scanCode, keyAction, keyModifiers);
	input->keyEvent(keyCode, keyAction, keyModifiers);
}

void TestRenderSurface::mouseButtonEvent(int buttonCode, int keyAction, int keyModifiers)
{
	GUIInterface::mouseButtonEvent(buttonCode, keyAction, keyModifiers);
	input->mouseButtonEvent(buttonCode, keyAction, keyModifiers);
}

void TestRenderSurface::mouseMoveEvent(float mouseX, float mouseY, float deltaX, float deltaY)
{
	GUIInterface::mouseMoveEvent(mouseX, mouseY, deltaX, deltaY);
	input->mouseMoveEvent(deltaX, deltaY);
}

void TestRenderSurface::scrollEvent(float deltaX, float deltaY)
{
	GUIInterface::scrollEvent(deltaX, deltaY);
	input->scrollEvent(deltaX, deltaY);
}
