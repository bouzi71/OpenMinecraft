#include "stdafx.h"

#include "Plane.h"

namespace terra
{
namespace math
{

Plane::Plane(glm::vec3 normal, float distance)
	: m_Normal(normal)
	, m_Distance(distance)
{}

Plane::Plane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	m_Normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
	m_Distance = glm::dot(m_Normal, p1);
}

float Plane::PointDistance(const glm::vec3& q) const
{
	return (glm::dot(m_Normal, q) - m_Distance) / glm::dot(m_Normal, m_Normal);
}

float Plane::PointDistance(glm::ivec3 q) const
{
	using T = glm::ivec3::value_type;

	glm::vec3 n(static_cast<T>(m_Normal.x), static_cast<T>(m_Normal.y), static_cast<T>(m_Normal.z));

	return static_cast<float>((glm::dot(n, glm::vec3(q)) - m_Distance) / glm::dot(n, n));
}

float Plane::PointDistance(glm::dvec3 q) const
{
	glm::dvec3 n(m_Normal.x, m_Normal.y, m_Normal.z);

	return static_cast<float>((glm::dot(n, q) - m_Distance) / glm::dot(n, n));
}

} // ns math
} // ns terra
