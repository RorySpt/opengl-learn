#include "stdafx.h"
#include "test_render_surface.h"

#include <stack>
#include <typeindex>

#include "actor_camera_component.h"
#include "actor_primitive_component.h"
#include "camera_actor.h"
#include "Model.h"
#include "player_controller.h"


class BoxPrimitiveComponent:public PrimitiveComponent
{
public:
	void draw(const Camera& camera) override
	{
		auto point_lights = GetOwner()->GetWorld()->GetLightsByType(LightSource::Point);
		if (point_lights.empty())
			model->setLight({});
		else
			model->setLight(point_lights[0].as<LightSource::Point>());

		model->setMaterial(_material);
		model->draw(camera, GetComponentToWorld());
	}
	Material3 _material;
	std::shared_ptr<BoxModel_SimpleTexture> model = comm::getOrCreate<BoxModel_SimpleTexture>();
};

unsigned GetRandomRabbitTexture();
class MyActor:public Actor
{
public:
	MyActor()
	{
		std::default_random_engine mt;
		
		auto container2_diffuse = comm::loadTexture(std::string(comm::dir_picture) + "/container2.png");
		auto container2_specular = comm::loadTexture(std::string(comm::dir_picture) + "/container2_specular.png");


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

			box_primitive->_material = { GetRandomRabbitTexture(),
			container2_diffuse
			,container2_specular
			,32.0f };
			box_primitives.emplace_back(box_primitive);
		}
		
	}


	std::vector<BoxPrimitiveComponent*> box_primitives;
};


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

	static bool bRelative = true;
	if(ImGui::SmallButton(bRelative ? "Relative" : "Absolute"))
	{
		bRelative = !bRelative;
	}

	ImGui::SetNextItemOpen(true);
	std::stack<std::string> level;
	level.push("World");
	if (ImGui::CollapsingHeader(level.top().c_str()))
	{
		ImGuiIO& io = ImGui::GetIO();

		for (auto actor : world._workActors)
		{
			// Foreach Actor
			auto actor_type_name = std::type_index(typeid(*actor)).name();
			level.push(actor_type_name);
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
						
						auto pos = bRelative ? component->GetRelativeLocation() : component->GetWorldLocation();
						auto rot = glm::degrees(convertToEulerAngle(bRelative ? component->GetRelativeRotation() : component->GetWorldRotation()).data);
						auto sca = bRelative ? component->GetRelativeScale3d() : component->GetWorldScale3d();
						auto qua = bRelative ? component->GetRelativeRotation() : component->GetWorldRotation();

						//ImGui::BulletText(std::format("Relative Position: {:.3f}, {:.3f}, {:.3f}", pos[0], pos[1], pos[2]).c_str());
						//ImGui::BulletText(std::format("Relative Rotation: {:.3f}, {:.3f}, {:.3f}", rot.data[0], rot.data[1], rot.data[2]).c_str());
						//ImGui::BulletText(std::format("Relative Scale3d : {:.3f}, {:.3f}, {:.3f}", sca[0], sca[1], sca[2]).c_str());
						if(ImGui::DragFloat3("Position", &pos[0], 0.1f))
						{
							if (bRelative)component->SetRelativeLocation(pos);
							else component->SetWorldLocation(pos);
						}
						ImGui::SameLine(); HelpMarker("opengl right-hand coordinate system, right as x, up as y, front as -z.");

						if (ImGui::DragFloat3("Rotation", &rot[0], 0.1f))
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
						if (ImGui::DragFloat3("Scale3d", &sca[0], 0.1f))
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
						ImGui::Checkbox("Lock", &relevance);
						if (ImGui::DragFloat4("Quaternion", &qua[0], 0.1f))
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
	Light light = { lightColor * lightRatio.x,lightColor * lightRatio.y,lightColor * lightRatio.z, lightWorldPos };

	world._lights[0] = LightSourcePoint(light);
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
