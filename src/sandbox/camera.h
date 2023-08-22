#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "common.h"
#include "euler_angle_utils.h"
#include "flags.h"
#include "imgui.h"


constexpr float CAMERA_DEFAULT_YAW = 0.f;
constexpr float CAMERA_DEFAULT_PITCH = 0.0f;
constexpr float CAMERA_DEFAULT_SPEED = 2.5f;
constexpr float CAMERA_DEFAULT_SPEED_MAX = 10.f;
constexpr float CAMERA_DEFAULT_SPEED_MIN = CAMERA_DEFAULT_SPEED;
constexpr float CAMERA_DEFAULT_SPEED_ACCELERATION = 10.f;
constexpr float CAMERA_DEFAULT_SPEED_DECELERATION = 30.f;
constexpr float CAMERA_DEFAULT_SENSITIVITY = 0.05f;
constexpr float CAMERA_DEFAULT_ZOOM = 45.0f;

constexpr glm::vec3 CAMERA_DEFAULT_POSITION = {};
constexpr glm::vec3 CAMERA_DEFAULT_UP = {0,1,0};
constexpr glm::vec3 CAMERA_DEFAULT_RIGHT = { 1,0,0 };
constexpr glm::vec3 CAMERA_DEFAULT_FRONT = { 0,0,-1 };
constexpr glm::vec3 CAMERA_DEFAULT_WORLD_UP = { 0,1,0 };

class Camera
{
public:
	enum Camera_KeyAction
	{
		IDLE	 = 0,
		FORWARD  = 1,
		BACKWARD = 1 << 1,
		LEFT	 = 1 << 2,
		RIGHT	 = 1 << 3,
		SPEED_UP = 1 << 4,
	};
	Q_DECLARE_FLAGS(Camera_KeyActions, Camera_KeyAction)

	// 摄像机属性
	glm::vec3 Position;	//位置
	glm::vec3 Front;	//朝向，用来指示运动方向, runtime variable, be calculated by Pitch and Yaw
	glm::vec3 Up;		//上方，runtime variable, be calculated
	glm::vec3 Right;	//右方，runtime variable, be calculated
	glm::vec3 WorldUp;	//世界上方
	// 欧拉角 
	/**
	 * \brief 世界坐标系下的，右手坐标系，角度
	 */
	float Yaw;			//欧拉角
	float Pitch;		//欧拉角
	// 摄像机设置
	float MovementSpeedRatio;	//移动速度倍率
	float MovementSpeedAcceleration;
	float MovementSpeedDeceleration;
	float MovementSpeed;		//移动速度
	float MovementSpeedMin;		//移动速度
	float MovementSpeedMax;		//移动速度
	
	float MouseSensitivity;		//相机灵敏度
	float Zoom;					//缩放

	int VWidth = 1200;
	int VHeight = 800;
	float ZFar = 100.f;	//远裁剪面
	float ZNear = 0.1f; //近裁剪面	

	Camera_KeyActions FrameKeyAction;

	bool bShowDebugWindow;

	Camera();
	
	[[nodiscard]] glm::mat4 getViewMatrix()const;
	[[nodiscard]] glm::mat4 getProjMatrix()const;

	void processKeyAction(Camera_KeyActions keyActions);
	void processMouseMove(float x_offset, float y_offset, GLboolean constrainPitch = true);
	void processMouseScroll(float);
	
	void resizeViewport(int w, int h);

	void update(float deltaTime);
	
private:
	void updateCameraVector();
	void updateCameraByKeyAction(float deltaTime);
	void drawDebugInfo();

	static glm::vec2 convertKeyActionsToVec2(Camera_KeyActions);
};

inline Camera::Camera()
	: Position(CAMERA_DEFAULT_POSITION), Front(CAMERA_DEFAULT_FRONT), Up(CAMERA_DEFAULT_UP),
	  Right(CAMERA_DEFAULT_RIGHT),
	  WorldUp(CAMERA_DEFAULT_WORLD_UP),
	  Yaw(CAMERA_DEFAULT_YAW),
	  Pitch(CAMERA_DEFAULT_PITCH),
	  MovementSpeedRatio(1.0), MovementSpeedAcceleration(CAMERA_DEFAULT_SPEED_ACCELERATION),
	  MovementSpeedDeceleration(CAMERA_DEFAULT_SPEED_DECELERATION),
	  MovementSpeed(CAMERA_DEFAULT_SPEED), MovementSpeedMin(CAMERA_DEFAULT_SPEED_MIN),
	  MovementSpeedMax(CAMERA_DEFAULT_SPEED_MAX),
	  MouseSensitivity(CAMERA_DEFAULT_SENSITIVITY),
	  Zoom(CAMERA_DEFAULT_ZOOM),
	  ZFar(100.f),
	  ZNear(0.1f), FrameKeyAction(IDLE), bShowDebugWindow(false)
{
	//updateCameraVector();
}

inline glm::mat4 Camera::getViewMatrix() const
{
	// inverse Local-To-World
	return mat4_cast(inverse(glm::quat({
		glm::radians(Pitch), glm::radians(Yaw), 0
	}))) * glm::translate(glm::mat4(1), -Position);
	//return glm::lookAt(Position, Position + Front, Up);
}

inline glm::mat4 Camera::getProjMatrix() const
{
	return glm::perspective(glm::radians(Zoom), static_cast<float>(VWidth) / static_cast<float>(VHeight), ZNear, ZFar);
}

inline void Camera::processKeyAction(Camera_KeyActions keyActions)
{
	if (keyActions.testFlag(LEFT) && keyActions.testFlag(RIGHT))
		keyActions &= ~LEFT & ~RIGHT;
	if (keyActions.testFlag(FORWARD) && keyActions.testFlag(BACKWARD))
		keyActions &= ~FORWARD & ~BACKWARD;
	FrameKeyAction |= keyActions;
}

inline void Camera::update(const float deltaTime)
{
	updateCameraVector();
	updateCameraByKeyAction(deltaTime);
	if(bShowDebugWindow)
		drawDebugInfo();
}

inline void Camera::updateCameraVector()
{
	//front.x = -cos(glm::radians(Pitch)) * sin(glm::radians(Yaw/* + 90*/));
	//front.y = sin(glm::radians(Pitch));
	//front.z = -cos(glm::radians(Pitch)) * cos(glm::radians(Yaw/* + 90*/));
	const glm::vec3 front = convertToVec3D(glm::radians(glm::vec3{Pitch, Yaw + 90, 0}));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));

	return;
	const glm::quat quaternion({ glm::radians(Pitch),glm::radians(Yaw),0 });
	//glm::dquat quaternion({ 0,0,-1 }, Front);
	auto eulerAngles = glm::degrees(glm::eulerAngles(quaternion));
	auto mat = glm::mat3_cast(quaternion);
	auto matInverse = glm::inverse(mat);

	glm::vec3 _Front = -mat[2];
	glm::vec3 _Right = mat[0];
	glm::vec3 _Up = mat[1];


}

inline void Camera::updateCameraByKeyAction(float deltaTime)
{
	const Camera_KeyActions actions = std::exchange(FrameKeyAction, IDLE);


	//MovementSpeedRatio = actions.testFlag(SPEED_UP) ? CAMERA_DEFAULT_SPEED_ACCELERATION : -CAMERA_DEFAULT_SPEED_DECELERATION;
	ImGui::Begin("Debug");
	MovementSpeed = actions.testFlag(SPEED_UP) ? MovementSpeedMax:MovementSpeedMin;
	
	//MovementSpeedRatio = actions.testFlag(SPEED_UP) ? 1.0f : 1.0f;
	//MovementSpeed = glm::clamp(MovementSpeed, CAMERA_DEFAULT_SPEED_MIN, CAMERA_DEFAULT_SPEED_MAX);
	const float velocity = MovementSpeed * MovementSpeedRatio;
	const glm::vec2 direction = convertKeyActionsToVec2(actions);
	//comm::print("direction: {}, {}\n", direction.x,direction.y);

	//std::string_view s("direction: {}, {}");
	//std::cout << std::format(std::string_view("direction: {}, {}"), direction.x, direction.y) << std::endl;
	if (actions & 0X0F)
	{
		const glm::vec3 moveDirection = normalize((Front * direction.y + Right * direction.x));
		Position += moveDirection * velocity * deltaTime;
	}
	
	ImGui::Text("%s", std::format("direction: {}, {}", direction.x, direction.y).c_str());
	ImGui::End();
}

inline void Camera::drawDebugInfo()
{
	Camera& camera = *this;
	ImGui::Begin("Camera",&bShowDebugWindow);

	ImGui::Text("camera current speed: %f", MovementSpeed * MovementSpeedRatio);
	ImGui::DragFloat3("camera current position", &Position[0],0.1);
	
	ImGui::SliderFloat("camera MovementSpeedRatio", &camera.MovementSpeedRatio, 1.0, 10);
	ImGui::SliderFloat("camera MovementSpeedMax", &camera.MovementSpeedMax, camera.MovementSpeedMin, 100);
	ImGui::SliderFloat("camera MovementNormalSpeed", &camera.MovementSpeedMin, 0, 100);

	ImGui::SliderFloat("camera ZFar", &camera.ZFar, camera.ZNear, 1000);
	ImGui::SliderFloat("camera ZNear", &camera.ZNear, 0, 100);

	ImGui::SliderFloat("camera Zoom", &camera.Zoom,10, 360);

	ImGui::SliderFloat("camera pitch", &camera.Pitch, -89, 89);
	auto inverseYaw = -camera.Yaw;
	ImGui::SliderFloat("camera -yaw", &inverseYaw, -180, 180);
	camera.Yaw = -inverseYaw;

	ImGui::Text("camera right-vector: % .5f, % .5f, % .5f", camera.Right.x, camera.Right.y, camera.Right.z);
	ImGui::Text("camera up-vector: % .5f, % .5f, % .5f", camera.Up.x, camera.Up.y, camera.Up.z);
	ImGui::Text("camera front-vector: % .5f, % .5f, % .5f", camera.Front.x, camera.Front.y, camera.Front.z);



	ImGui::End();
}

inline glm::vec2 Camera::convertKeyActionsToVec2(const Camera_KeyActions direction)
{
	return {
		static_cast<float>(direction.testFlag(RIGHT) - direction.testFlag(LEFT)),
		static_cast<float>(direction.testFlag(FORWARD) - direction.testFlag(BACKWARD))
	};
}

inline void Camera::processMouseMove(float x_offset, float y_offset,const GLboolean constrainPitch)
{
	x_offset *= MouseSensitivity;
	y_offset *= MouseSensitivity;
	Yaw -= x_offset;
	Pitch += y_offset;

	Yaw = math::NormalizeAngleDegrees(Yaw);
	if(constrainPitch)
	{
		if (Pitch > 89.0f)Pitch = 89.0f;
		if (Pitch < -89.0f)Pitch = -89.0f;
	}
	//updateCameraVector();
}

inline void Camera::processMouseScroll(const float y_offset)
{
	Zoom -= y_offset;
	if (Zoom < 1.0f)Zoom = 1.0f;
	if (Zoom > 45.0f)Zoom = 45.0f;
}

inline void Camera::resizeViewport(int w, int h)
{
	if(w == 0||h == 0)return;
	VWidth = w;
	VHeight = h;
}
