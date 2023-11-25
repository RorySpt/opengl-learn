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
//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
struct ExampleAppLog
{
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
	bool                AutoScroll;  // Keep scrolling if already at the bottom.

	ExampleAppLog()
	{
		AutoScroll = true;
		Clear();
	}

	void    Clear()
	{
		Buf.clear();
		LineOffsets.clear();
		LineOffsets.push_back(0);
	}

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
	{
		int old_size = Buf.size();
		va_list args;
		va_start(args, fmt);
		Buf.appendfv(fmt, args);
		va_end(args);
		for (int new_size = Buf.size(); old_size < new_size; old_size++)
			if (Buf[old_size] == '\n')
				LineOffsets.push_back(old_size + 1);
	}

	void    Draw()
	{
		// Options menu
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &AutoScroll);
			ImGui::EndPopup();
		}

		// Main window
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		bool clear = ImGui::Button("Clear");
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		Filter.Draw("Filter", -100.0f);

		ImGui::Separator();
		//if (ImGui::BeginChild("scrolling", ImVec2(0, ImGui::GetTextLineHeight() * (LineOffsets.size() + 1)), false, ImGuiWindowFlags_HorizontalScrollbar| ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			if (Filter.IsActive())
			{
				// In this example we don't use the clipper when Filter is enabled.
				// This is because we don't have random access to the result of our filter.
				// A real application processing logs with ten of thousands of entries may want to store the result of
				// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
				for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					if (Filter.PassFilter(line_start, line_end))
					{
						ImGui::TextWrapped("%s", std::string(line_start,line_end).c_str());
					}
						
				}
			}
			else
			{
				// The simplest and easy way to display the entire buffer:
				//   ImGui::TextUnformatted(buf_begin, buf_end);
				// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
				// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
				// within the visible area.
				// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
				// on your side is recommended. Using ImGuiListClipper requires
				// - A) random access into your data
				// - B) items all being the  same height,
				// both of which we can handle since we have an array pointing to the beginning of each line of text.
				// When using the filter (in the block of code above) we don't have random access into the data to display
				// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
				// it possible (and would be recommended if you want to search through tens of thousands of entries).
				ImGuiListClipper clipper;
				clipper.Begin(LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::Text("%s", std::string(line_start, line_end).c_str());
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
			// Using a scrollbar or mouse-wheel will take away from the bottom edge.
			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}
		//ImGui::EndChild();
	}
};

void TestRenderSurface::UI_Scene()
{
	static ExampleAppLog log;
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize);
	int windowPosX, windowPosY;
	glfwGetWindowPos(_window, &windowPosX, &windowPosY);
	ImGui::SetWindowPos(ImVec2(windowPosX, windowPosY));
	float height = ImGui::GetTextLineHeightWithSpacing();
	for(int i = 0; i < 100; ++i)
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(windowPosX, windowPosY + i * height), 0xff0000ff, "Hello World");
	
	ImGui::End();
	
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
