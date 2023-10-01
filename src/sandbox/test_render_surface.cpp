#include "stdafx.h"
#include "test_render_surface.h"

#include <execution>
#include <stack>
#include <typeindex>

#include "actor_camera_component.h"
#include "actor_light_component.h"
#include "actor_primitive_component.h"
#include "camera_actor.h"
#include "display_window_private.h"
#include "player_controller.h"
#include "texture_loader.h"
#include "render/CustomActors.h"


template<typename ...Ty>
bool ScopeColor_TreeNode(ImVec4 color_f, Ty... args)
{
	ImGui::PushStyleColor(ImGuiCol_Text, color_f);
	auto ret = ImGui::TreeNode(std::forward<Ty>(args)...);
	ImGui::PopStyleColor();
	return ret;
}

template<typename Ret, typename ...Ty>
Ret ScopeColor(ImVec4 color_f, Ret(*func)(Ty...), Ty... args)
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
template<typename T> requires !requires(T* obj) { obj->UI_Draw(); }
void DrawUI(T* object)
{
}

template<typename T> requires requires(T* obj) { obj->UI_Draw(); }
void DrawUI(T* object)
{
	object->UI_Draw();
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
				DrawUI(actor);
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


							ImGui::SameLine(0, 100);
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
							if (ImGui::DragFloat3((tr_type + "Position").c_str(), &pos[0], 0.1f))
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
								if (relevance)
								{
									float value;
									for (int n = 0; n < 3; ++n)
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
							DrawUI(component);
							// CameraComponent
							{
								if (const auto camera_component = dynamic_cast<CameraComponent*>(component))
								{
									auto _loc_world = camera_component->GetComponentToWorld();
									ImGui::Text("%s", std::format("Camera Position: {:.3f}, {:.3f}, {:.3f}"
										, _loc_world[3][0], _loc_world[3][1], _loc_world[3][2]).c_str());
								}

							}
							//// LightComponent
							//{
							//	if (const auto camera_component = dynamic_cast<LightComponent*>(component))
							//	{
							//		DrawUI(camera_component);
							//	}
							//
							//}
							//// SpherePrimitiveComponent
							//{
							//	if (const auto camera_component = dynamic_cast<SpherePrimitiveComponent*>(component))
							//	{
							//		camera_component->UI_Draw();
							//	}
							//}

							if (component->GetNumChildrenComponents() > 0)
								ImGui::SeparatorText(std::format("SubComponents").c_str());
							for (const auto sub : component->GetAttachChildren())
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
	g_debug_draw = std::make_shared<DebugDraw>();
	camera_manager = world.GetPlayerController()->GetCameraManager();
	input = world.GetPlayerController()->GetInputManager();
	input->EnableInput();

	world.SpawnActor<SpaceBoxActor>();
	world.SpawnActor<CameraActor>();

	world.SpawnActor<SnowFlakeActor>();

	//auto mesh = world.SpawnActor<Actor>();
	//mesh->set_name("MyMesh");
	//mesh = world.SpawnActor<Actor>();
	//mesh->set_name("MyMesh");

	//world.SpawnActor<MyActor>();

	static glm::vec3 lightColor(1, 1, 1);
	static glm::vec3 lightRatio(0.3f, 0.7f, 1);



	glm::vec3 lightWorldPos = glm::vec4{ 1.2f, 5.2f, 10.0f, 1.0f } *2.0f;


	//std::cout << typeid(LightActor).name() << std::endl;
	// ¹âÔ´
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

	delegate_handle_handler<object>::tick_handle();
}

void TestRenderSurface::draw(float deltaTime)
{
	UI_Scene();
	const auto camera = world.GetPlayerController()->GetCameraManager()->ActivatedCamera().lock();

	// Gets Pointers to all primitive
	auto primitives = world._workActors
		| std::views::transform([](Actor* actor)
			{
				return actor->_owned_components
					| std::views::transform([](const std::unique_ptr<ActorComponent>& component)
						{return dynamic_cast<PrimitiveComponent*>(component.get()); })
					| std::views::filter([](const PrimitiveComponent* com) {return com != nullptr; });
			})
		| std::views::join;

	// Filter out those that need blend and those that don't
	auto need_blend_primitives_ranges = primitives
		| std::views::filter([](const PrimitiveComponent* com) {return com->bHasBlend; });
	auto opaques_primitive_ranges = primitives
		| std::views::filter([](const PrimitiveComponent* com) {return !com->bHasBlend; });

	// Sort the primitives that need to blend by distance
	auto need_blend_primitives = need_blend_primitives_ranges
		| std::ranges::to<std::vector<PrimitiveComponent*>>();

	std::ranges::sort(need_blend_primitives,
		[distance_to_camera = [camera_location = glm::vec3(inverse(camera->getViewMatrix())[3])]
		(const PrimitiveComponent* com)
		{
			return glm::length(com->GetWorldLocation() - camera_location);
		}](const PrimitiveComponent* left, const PrimitiveComponent* right)
		{
			return distance_to_camera(left) > distance_to_camera(right);
		});

	glDisable(GL_BLEND);
	// Start by drawing entities that don't need blend
	for (const auto primitive : opaques_primitive_ranges | std::views::all)
	{
		primitive->draw(*camera);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Then draw the entities that need blend
	for (const auto primitive : need_blend_primitives)
	{
		primitive->draw(*camera);
	}

	//if (!debugDraw)debugDraw = std::make_shared<DebugDraw>();
	g_debug_draw->DrawLine(*camera, { glm::vec3(0,0,0), glm::vec3(0,0,100) }, { 0,0,1,1 });
	g_debug_draw->DrawLine(*camera, { glm::vec3(0,0,0), glm::vec3(100,0,0) }, { 1,0,0,1 });
	g_debug_draw->DrawLine(*camera, { glm::vec3(0,0,0), glm::vec3(0,100,0) }, { 0,1,0,1 });
	g_debug_draw->DrawBox(*camera, DebugDraw::ExtentBox(), { 1,0,1,1 });

}
