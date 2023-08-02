#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define _GL_MATH_BEGIN inline namespace math{
#define _GL_MATH_END }
#define _GL_MATH ::math::

_GL_MATH_BEGIN


// The default unit should be radius
struct EulerAngle  // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	union
	{
		struct { float pitch, yaw, roll; };   // NOLINT(clang-diagnostic-nested-anon-types)
		glm::vec3 data;
	};

public:
	EulerAngle() = default;
	EulerAngle(const float pitch, const float yaw, const float roll)  // NOLINT(cppcoreguidelines-pro-type-member-init)
		: pitch(pitch),
		yaw(yaw),
		roll(roll)
	{
	}

	EulerAngle(const glm::vec3& data)  // NOLINT(cppcoreguidelines-pro-type-member-init)
		: data(data)
	{
	}

	operator glm::vec3() const
	{
		return data;
	}
};
inline glm::vec3 convertToVec3D(const EulerAngle euler)
{
	const float sp = sin(euler.pitch),
		cp = cos(euler.pitch),
		sy = sin(euler.yaw),
		cy = cos(euler.yaw);
	return glm::vec3{
		cp* cy,
			sp,
			cp * -sy
	};
}

inline EulerAngle convertToQuaternion_(const EulerAngle euler)
{
	constexpr auto pi = glm::pi<float>();
	EulerAngle euler_ = euler;
	if (euler_.yaw > pi / 2)
	{
		euler_.yaw -= pi;
		euler_.pitch = (euler_.pitch > 0 ? -pi : pi) - euler_.pitch;
		euler_.roll += euler_.roll > 0 ? -pi : pi;
	}
	else if (euler_.yaw <= -pi / 2)
	{
		euler_.yaw += pi;
		euler_.pitch = (euler_.pitch > 0 ? -pi : pi) - euler_.pitch;
		euler_.roll += euler_.roll > 0 ? -pi : pi;
	}
	return { euler_ };
}
// glm 原生转换，当yaw超过[-90,90)的值会被错误转换
inline glm::quat convertToQuaternion_native(const EulerAngle euler)
{
	EulerAngle euler_ = euler;

	return { euler };
}
// glm 原生转换，会限制yaw到[-90,90)
inline EulerAngle convertToEulerAngle_native(glm::quat qua)
{
	EulerAngle euler_ = eulerAngles(qua);

	return { euler_ };
}

inline glm::quat convertToQuaternion(const EulerAngle euler)
{
	constexpr auto pi = glm::pi<float>();
	EulerAngle euler_ = euler;
	if (euler_.yaw > pi / 2)
	{
		euler_.yaw -= pi;
		euler_.pitch = (euler_.pitch > 0 ? pi : -pi) - euler_.pitch;
		euler_.roll += euler_.roll > 0 ? -pi : pi;
	}
	else if (euler_.yaw <= -pi / 2)
	{
		euler_.yaw += pi;
		euler_.pitch = (euler_.pitch > 0 ? pi : -pi) - euler_.pitch;
		euler_.roll += euler_.roll > 0 ? -pi : pi;
	}
	return { euler_ };
}
// 将四元数转换为欧拉角，欧拉角顺序为pitch-yaw-roll，数值约束为pitch[-90,90]，yaw[-180,180]，roll[-180,180]
inline EulerAngle convertToEulerAngle(glm::quat qua)
{
	constexpr auto pi = glm::pi<float>();

	EulerAngle euler_ = eulerAngles(qua);
	if (euler_.pitch >= pi / 2)
	{
		euler_.pitch = pi - euler_.pitch;
		euler_.yaw += euler_.yaw > 0 ? -pi : pi;
		euler_.roll += euler_.roll > 0 ? -pi : pi;
	}
	else if (euler_.pitch < -pi / 2)
	{
		euler_.pitch = -pi - euler_.pitch;
		euler_.yaw += euler_.yaw > 0 ? -pi : pi;
		euler_.roll += euler_.roll > 0 ? -pi : pi;
	}
	return { euler_ };
}
inline EulerAngle convertToEulerAngle(glm::vec3 direction)
{
	const float pitch = atan2(direction.y, sqrt(direction.x * direction.x + direction.z * direction.z));
	const float yaw = atan2(-direction.z, direction.x);
	return {
			pitch ,yaw,0
	};
}
//template<typename T>
//concept ConceptAngle = 

template<typename T>
T NormalizeAngle(const T radius) requires std::numeric_limits<T>::is_iec559
{
	constexpr auto pi = glm::pi<T>();
	T a = std::fmod(radius + pi, 2 * pi);
	if (a < 0.0) {
		a += 2 * pi;
	}
	return a - pi;
}
template<typename T>
T NormalizeAngleDegrees(const T degrees) requires std::numeric_limits<T>::is_iec559
{
	constexpr T max = 180;
	T a = std::fmod(degrees + max, 2 * max);
	if (a < 0) {
		a += 2 * max;
	}
	return a - max;
}

_GL_MATH_END