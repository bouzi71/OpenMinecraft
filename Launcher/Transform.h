#pragma once

#ifndef TERRACOTTA_TRANSFORM_H_
#define TERRACOTTA_TRANSFORM_H_

#include <common/Vector.h>
#include <common/AABB.h>

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

	CMinecraftAABB bounding_box;
};

#endif
