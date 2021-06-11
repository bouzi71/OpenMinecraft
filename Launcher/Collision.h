#pragma once

#ifndef TERRACOTTA_COLLISION_H_
#define TERRACOTTA_COLLISION_H_

#include "Transform.h"
#include <world/World.h>

class Collision
{
private:
	glm::dvec3 m_Position;
	glm::dvec3 m_Normal;

public:
	Collision() noexcept {}
	Collision(glm::dvec3 position, glm::dvec3 normal) noexcept : m_Position(position), m_Normal(normal) {}

	glm::dvec3 GetPosition() const noexcept { return m_Position; }
	glm::dvec3 GetNormal() const noexcept { return m_Normal; }
};

class CollisionDetector
{
private:
	World* m_World;

	std::vector<glm::ivec3> GetSurroundingLocations(CMinecraftAABB bounds);

public:
	CollisionDetector(World* world) noexcept : m_World(world) {}

	bool DetectCollision(glm::dvec3 from, glm::dvec3 rayVector, Collision* collision) const;

	void ResolveCollisions(Transform* transform, double dt, bool* onGround);
};


#endif
