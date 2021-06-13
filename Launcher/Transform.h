#pragma once

#ifndef TERRACOTTA_TRANSFORM_H_
#define TERRACOTTA_TRANSFORM_H_

#include <common/Vector.h>
#include <common/AABB.h>

struct Transform
{
	glm::dvec3 position;
	glm::dvec3 velocity;
	glm::dvec3 input_velocity;
	glm::dvec3 acceleration;
	glm::dvec3 input_acceleration;

	float max_speed;
	float orientation;
	float rotation;

	BoundingBox bounding_box;
};

#endif
