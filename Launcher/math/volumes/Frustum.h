#ifndef TERRACOTTA_MATH_VOLUMES_FRUSTUM_H_
#define TERRACOTTA_MATH_VOLUMES_FRUSTUM_H_

#include <common/AABB.h>
#include "../Plane.h"

namespace terra
{
	namespace math
	{
		namespace volumes
		{

			class Frustum
			{
			public:
				Frustum(glm::vec3 position, glm::vec3 forward, float near, float far, float fov, float ratio, glm::vec3 up, glm::vec3 right);

				bool Intersects(const CMinecraftAABB& aabb) const;
				bool Intersects(glm::vec3 v) const;
				bool Intersects(Vector3i v) const;
				bool Intersects(Vector3d v) const;

			private:
				glm::vec3 m_Position;
				glm::vec3 m_Forward;
				float m_Near;
				float m_Far;
				float m_NearWidth;
				float m_NearHeight;
				float m_FarWidth;
				float m_FarHeight;

				Plane m_Planes[6];
			};

		} // ns volumes
	} // ns math
} // ns terra

#endif
