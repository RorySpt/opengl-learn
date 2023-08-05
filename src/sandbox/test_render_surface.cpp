#include "stdafx.h"
#include "test_render_surface.h"

#include <stack>
#include <typeindex>

#include "actor_camera_component.h"
#include "camera_actor.h"
#include "Model.h"
#include "player_controller.h"



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
							if (bRelative)component->SetRelativeRotation(convertToQuaternion( glm::radians(rot)));
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


						if (ImGui::DragFloat3("Scale3d", &sca[0], 0.1f))
						{
							if (bRelative)component->SetRelativeScale3d(sca);
							else component->SetWorldScale3d(sca);
						}
						if (ImGui::DragFloat4("Quaternion", &qua[0], 0.1f))
						{
							if (bRelative)component->SetRelativeRotation(qua);
							else component->SetRelativeRotation(qua);
						}



						if (const auto camera_component = dynamic_cast<CameraComponent*>(component))
						{
							auto _loc_world = camera_component->GetComponentToWorld();
							ImGui::Text("%s", std::format("Camera Position: {:.3f}, {:.3f}, {:.3f}"
								, _loc_world[3][0], _loc_world[3][1], _loc_world[3][2]).c_str());
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
