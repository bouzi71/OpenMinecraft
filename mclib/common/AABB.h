#ifndef MCLIB_COMMON_AABB_H_
#define MCLIB_COMMON_AABB_H_

#if 0

#include <common/Vector.h>

#include <limits>
#include <algorithm>

#undef min
#undef max


struct CMinecraftAABB
{
	glm::dvec3 min;
	glm::dvec3 max;

	MCLIB_API CMinecraftAABB() noexcept 
		: min(0, 0, 0)
		, max(0, 0, 0) 
	{}
	MCLIB_API CMinecraftAABB(const glm::dvec3& min, const glm::dvec3& max) noexcept 
		: min(min)
		, max(max) 
	{}

	bool MCLIB_API Contains(glm::dvec3 point) const noexcept
	{
		return (point.x >= min.x && point.x <= max.x) &&
			(point.y >= min.y && point.y <= max.y) &&
			(point.z >= min.z && point.z <= max.z);
	}

	CMinecraftAABB operator+(glm::dvec3 offset) const
	{
		return CMinecraftAABB(min + offset, max + offset);
	}

	CMinecraftAABB operator+(glm::ivec3 offset) const
	{
		return CMinecraftAABB(min + glm::dvec3(offset), max + glm::dvec3(offset));
	}

	bool MCLIB_API Intersects(const CMinecraftAABB& other) const noexcept
	{
		return (max.x > other.min.x &&
			min.x < other.max.x &&
			max.y > other.min.y &&
			min.y < other.max.y &&
			max.z > other.min.z &&
			min.z < other.max.z);
	}

	bool MCLIB_API Intersects(const Ray& ray, double* length) const noexcept
	{
		double t1 = (min.x - ray.GetOrigin().x) * ray.GetReciprocal().x;
		double t2 = (max.x - ray.GetOrigin().x) * ray.GetReciprocal().x;

		double t3 = (min.y - ray.GetOrigin().y) * ray.GetReciprocal().y;
		double t4 = (max.y - ray.GetOrigin().y) * ray.GetReciprocal().y;

		double t5 = (min.z - ray.GetOrigin().z) * ray.GetReciprocal().z;
		double t6 = (max.z - ray.GetOrigin().z) * ray.GetReciprocal().z;

		using std::min;
		using std::max;

		double tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
		double tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

		if (tmax < 0)
		{
			if (length)
				*length = tmax;
			return false;
		}

		if (tmin > tmax)
		{
			if (length)
				*length = tmax;
			return false;
		}

		if (length)
			*length = tmin;
		return true;
	}
};

#endif

#endif
