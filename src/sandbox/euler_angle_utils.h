#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define _GL_MATH_BEGIN inline namespace math{
#define _GL_MATH_END }
#define _GL_MATH ::math::

_GL_MATH_BEGIN
/*
 * 坐标系定义：
 * 前：z负方向
 * 右：x正方形
 * 上：y正方向
 * 旋转定义：
 * 逆时针为正
 * 无序欧拉角定义
 * pitch：与x-z平面的夹角
 * yaw：绕y轴的旋转
 * roll: 绕自身x轴的旋转
 * 即用pitch，yaw表示四元数的旋转轴，roll表示四元数的旋转值
 */

// The default unit should be radius
struct EulerAngle  // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	using value_type = glm::vec3::value_type;

	union
	{
		struct { value_type pitch, yaw, roll; };   // NOLINT(clang-diagnostic-nested-anon-types)
		glm::vec3 data;
	};

	EulerAngle() = default;
	EulerAngle(const value_type pitch, const value_type yaw, const value_type roll)  // NOLINT(cppcoreguidelines-pro-type-member-init)
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
	value_type& operator [](const int i)
	{
		return data[i];
	}
	value_type operator [](const int i) const
	{
		return data[i];
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
	if (euler_.yaw >= pi / 2)
	{
		euler_.yaw -= pi;
		euler_.pitch = (euler_.pitch > 0 ? -pi : pi) - euler_.pitch;
		euler_.roll += euler_.roll > 0 ? -pi : pi;
	}
	else if (euler_.yaw < -pi / 2)
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

inline bool isValid(float v)
{
	bool b = isinf(v);
	bool b1 = isnan(v);
	bool b2 = !b && !b1;
	return b2;
}
inline bool isValid(glm::quat v)
{
	return isValid(v[0]) && isValid(v[1]) && isValid(v[2]) && isValid(v[3]);
}
inline bool isValid(glm::vec3 v)
{
	return isValid(v[0]) && isValid(v[1]) && isValid(v[2]);
}
inline bool isValid(glm::dvec3 v)
{
	return isValid(v[0]) && isValid(v[1]) && isValid(v[2]);
}

// glm 原生转换，会限制yaw到[-90,90)
inline EulerAngle convertToEulerAngle_native(glm::quat qua)
{
	
	EulerAngle euler_ = eulerAngles(qua);

	return { euler_ };
}

inline glm::quat convertToQuaternion(const EulerAngle euler)
{
	//assert(isValid(euler));
	//const auto rr = glm::rotate(glm::quat(1, 0, 0, 0), euler.roll, { 0,0,1 });
	//const auto rp = glm::rotate(glm::quat(1, 0, 0, 0), euler.pitch, glm::vec3{ 1, 0, 0 });
	//const auto ry = glm::rotate(glm::quat(1, 0, 0, 0), euler.yaw, { 0,1,0 });
	//
	//assert(isValid(ry * rp * rr));
	//return ry * rp * rr;

	//auto eulerAngle = euler.data;
	//
	//glm::vec3 c = glm::cos(eulerAngle * (0.5f));
	//glm::vec3 s = glm::sin(eulerAngle * (0.5f));
	//
	//float w = c.x * s.y * s.z + s.x * c.y * c.z;
	//float x = s.x * c.y * c.z + s.x * s.y * c.z;
	//float y = c.x * s.y * c.z - s.x * c.y * s.z;
	//float z = c.x * c.y * c.z - s.x * s.y * s.z;
	//
	auto qua = glm::quat({ euler.roll,euler.pitch,euler.yaw });
	return {qua.w,qua.y,qua.z,qua.x};
}

// 将四元数转换为欧拉角，欧拉角顺序为roll-pitch-yaw，数值约束为pitch[-90,90]
inline EulerAngle convertToEulerAngle(glm::quat qua)
{
	//constexpr glm::vec3 world_up = { 0,1,0 };
	//auto rMat = glm::mat3_cast(qua);
	//auto& [axisX, axisY, axisZ] = reinterpret_cast<std::array<glm::vec3, 3>&>(rMat[0]);
	//
	//
	//const auto forward = -axisZ;
	////const auto right = glm::cross(forward, world_up);
	////const auto up = glm::normalize(glm::cross(right, forward));
	////glm::roll()
	//
	//const float forward_proj_xz = sqrt(forward.x * forward.x + forward.z * forward.z);
	//const float pitch = atan2(forward.y, forward_proj_xz);
	//const float yaw = -atan2(forward.z, forward.x) - glm::pi<float>() / 2;
	//
	//auto rot_yaw = glm::rotate(glm::quat(1, 0, 0, 0), yaw, { 0,1,0 });
	//auto rot_pitch = glm::rotate(glm::quat(1, 0, 0, 0), pitch, { 1, 0, 0 });
	//auto rot_roll = inverse((rot_yaw * rot_pitch )) * qua;
	//
	////const auto f = glm::cross(up, axisY);
	//const auto roll = glm::angle(rot_roll);//acos(glm::clamp(dot(up, axisY), -1.0f, 1.0f));
	//
	////if (!isValid(roll)) roll = 0;
	//
	//assert(isValid(glm::vec3{ pitch, yaw, roll }));





	//return {pitch, yaw, roll };
	auto euler = glm::eulerAngles(glm::quat{qua.w, qua.z, qua.x, qua.y});
	return {euler[1],euler[2],euler[0]};
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