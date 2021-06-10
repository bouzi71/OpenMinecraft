#pragma once

#ifndef TERRACOTTA_COLLISION_H_
#define TERRACOTTA_COLLISION_H_

#include "Transform.h"
#include <world/World.h>

namespace terra
{

	class Collision
	{
	private:
		Vector3d m_Position;
		Vector3d m_Normal;

	public:
		Collision() noexcept {}
		Collision(Vector3d position, Vector3d normal) noexcept : m_Position(position), m_Normal(normal) {}

		Vector3d GetPosition() const noexcept { return m_Position; }
		Vector3d GetNormal() const noexcept { return m_Normal; }
	};

	class CollisionDetector
	{
	private:
		World* m_World;

		std::vector<Vector3i> GetSurroundingLocations(CMinecraftAABB bounds);

	public:
		CollisionDetector(World* world) noexcept : m_World(world) {}

		bool DetectCollision(Vector3d from, Vector3d rayVector, Collision* collision) const;

		void ResolveCollisions(Transform* transform, double dt, bool* onGround);
	};

} // namespace terra

#endif
