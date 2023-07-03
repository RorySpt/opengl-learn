#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define _GL_MATH_BEGIN inline namespace math{
#define _GL_MATH_END }
#define _GL_MATH ::math::

_GL_MATH_BEGIN


// The default unit should be radius
struct EulerAngle
{
	union
	{
		struct { float pitch, yaw, row; };  // NOLINT(clang-diagnostic-nested-anon-types)
		glm::vec3 data;
	};

public:
	EulerAngle(const float pitch, const float yaw, const float row)
		: pitch(pitch),
		  yaw(yaw),
		  row(row)
	{
	}

	EulerAngle(const glm::vec3& data)
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
		cp * cy,
		sp,
		cp * -sy
	};
}
inline glm::quat convertToQuaternion(const EulerAngle euler)
{
	return { euler };
}
inline EulerAngle convertToEulerAngle(glm::quat qua)
{
	return { eulerAngles(qua) };
}
inline EulerAngle convertToEulerAngle(glm::vec3 direction)
{
	const float pitch = atan2(direction.y , sqrt(direction.x * direction.x + direction.z * direction.z));
	const float yaw = atan2(-direction.z , direction.x);
	return {
			pitch ,yaw,0
		};
}
//template<typename T>
//concept ConceptAngle = 

template<typename T>
T NormalizeAngle(const T radius) requires std::numeric_limits<T>::is_iec559
{
	constexpr T M_PI = glm::pi<T>();
	T a = std::fmod(radius + M_PI, 2 * M_PI);
    if (a < 0.0) {
        a += 2 * M_PI;
    }
    return a - M_PI;
}
template<typename T>
T NormalizeAngleDegrees(const T degrees) requires std::numeric_limits<T>::is_iec559
{
	constexpr T Max = 180;
	T a = std::fmod(degrees + Max, 2 * Max);
	if (a < 0) {
		a += 2 * Max;
	}
	return a - Max;
}

_GL_MATH_END