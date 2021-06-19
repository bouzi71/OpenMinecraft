#pragma once

#ifndef TERRACOTTA_TRANSFORM_H_
#define TERRACOTTA_TRANSFORM_H_

#include <common/Vector.h>

struct Transform
{
	Transform()
		: bounding_box(glm::vec3(0.0f), glm::vec3(0.0f))
	{}

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
