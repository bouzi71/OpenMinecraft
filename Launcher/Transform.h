#pragma once

#ifndef TERRACOTTA_TRANSFORM_H_
#define TERRACOTTA_TRANSFORM_H_

#include <mclib/common/Vector.h>
#include <mclib/common/AABB.h>

namespace terra
{

struct Transform
{
	Vector3d position;
	Vector3d velocity;
	Vector3d input_velocity;
	Vector3d acceleration;
	Vector3d input_acceleration;
	float max_speed;
	float orientation;
	float rotation;

	AABB bounding_box;
};

} // namespace terra

#endif