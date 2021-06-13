#include "stdafx.h"

#include "Collision.h"
#include "Transform.h"

#include <iostream>

template <typename T>
inline T Sign(T val)
{
	return std::signbit(val) ? static_cast<T>(-1) : static_cast<T>(1);
}

inline glm::dvec3 BasisAxis(int basisIndex)
{
	static const glm::dvec3 axes[3] = { glm::dvec3(1, 0, 0), glm::dvec3(0, 1, 0), glm::dvec3(0, 0, 1) };
	return axes[basisIndex];
}

glm::dvec3 GetClosestFaceNormal(const glm::dvec3& pos, BoundingBox bounds)
{
	glm::dvec3 center = glm::vec3(bounds.getMin()) + glm::vec3(bounds.getMax() - bounds.getMin()) / 2.0f;
	glm::dvec3 dim = bounds.getMax() - bounds.getMin();
	glm::dvec3 offset = pos - center;

	double minDist = std::numeric_limits<double>::max();
	glm::dvec3 normal;

	for (int i = 0; i < 3; ++i)
	{
		double dist = dim[i] - std::abs(offset[i]);
		if (dist < minDist)
		{
			minDist = dist;
			normal = BasisAxis(i) * Sign(offset[i]);
		}
	}

	return normal;
}


bool CollisionDetector::DetectCollision(glm::dvec3 from, glm::dvec3 rayVector, Collision* collision) const
{
	static const std::vector<glm::dvec3> directions = {
		glm::dvec3(0, 0, 0), glm::dvec3(1, 0, 0), glm::dvec3(-1, 0, 0), glm::dvec3(0, 1, 0), glm::dvec3(0, -1, 0), glm::dvec3(0, 0, 1), glm::dvec3(0, 0, -1)
	};

	glm::dvec3 direction = glm::normalize(rayVector);
	double length = glm::length(rayVector);

	Ray ray(from, direction);

	if (collision)
		* collision = Collision();

	for (double i = 0; i < length; ++i)
	{
		glm::dvec3 position = from + direction * i;

		// Look for collisions in any blocks surrounding the ray
		for (glm::dvec3 checkDirection : directions)
		{
			glm::dvec3 checkPos = position + checkDirection;
			const CMinecraftBlock* block = m_World->GetBlock(checkPos);

			if (block && block->IsSolid())
			{
				BoundingBox bounds = block->GetBoundingBox(checkPos);
				double distance;

				if (bounds.Intersects(ray, &distance))
				{
					if (distance < 0 || distance > length) continue;

					glm::dvec3 collisionHit = from + direction * distance;
					glm::dvec3 normal = GetClosestFaceNormal(collisionHit, bounds);

					if (collision)
						* collision = Collision(collisionHit, normal);

					return true;
				}
			}
		}
	}

	return false;
}

std::vector<glm::ivec3> CollisionDetector::GetSurroundingLocations(BoundingBox bounds)
{
	std::vector<glm::ivec3> locs;

	int32 radius = 2;
	for (int32 y = (int32)bounds.getMin().y - radius; y < (int32)bounds.getMax().y + radius; ++y)
	{
		for (int32 z = (int32)bounds.getMin().z - radius; z < (int32)bounds.getMax().z + radius; ++z)
		{
			for (int32 x = (int32)bounds.getMin().x - radius; x < (int32)bounds.getMax().x + radius; ++x)
			{
				locs.emplace_back(x, y, z);
			}
		}
	}

	return locs;
}

void CollisionDetector::ResolveCollisions(Transform* transform, double dt, bool* onGround)
{
	const int32 MaxIterations = 10;
	bool collisions = true;

	glm::dvec3 velocity = transform->velocity;
	glm::dvec3 input_velocity = transform->input_velocity;

	for (int32 iteration = 0; iteration < MaxIterations && collisions; ++iteration)
	{
		glm::dvec3 position = transform->position;

		collisions = false;

		for (std::size_t i = 0; i < 3; ++i)
		{
			BoundingBox playerBounds = transform->bounding_box;

			if (iteration == 0)
				position[i] += velocity[i] * dt + input_velocity[i] * dt;

			playerBounds.setMin(playerBounds.getMin() + glm::vec3(position));
			playerBounds.setMax(playerBounds.getMax() + glm::vec3(position));

			std::vector<glm::ivec3> surrounding = GetSurroundingLocations(playerBounds);

			for (glm::ivec3 checkPos : surrounding)
			{
				const CMinecraftBlock* block = m_World->GetBlock(checkPos);

				if (block && block->IsSolid())
				{
					BoundingBox blockBounds = block->GetBoundingBox(checkPos);

					if (playerBounds.Intersects(blockBounds))
					{
						if (onGround != nullptr && i == 1 && checkPos.y < transform->position.y)
						{
							*onGround = true;
						}

						velocity[i] = 0;
						input_velocity[i] = 0;

						double penetrationDepth;

						if (playerBounds.getMin()[i] < blockBounds.getMin()[i])
						{
							penetrationDepth = playerBounds.getMax()[i] - blockBounds.getMin()[i];
						}
						else
						{
							penetrationDepth = playerBounds.getMin()[i] - blockBounds.getMax()[i];
						}

						position[i] -= penetrationDepth;
						collisions = true;
						break;
					}
				}
			}
		}

		transform->position = position;
	}

	transform->velocity = velocity;
	transform->input_velocity = input_velocity;
}
