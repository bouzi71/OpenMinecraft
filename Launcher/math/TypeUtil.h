#ifndef TERRACOTTA_MATH_TYPEUTIL_H_
#define TERRACOTTA_MATH_TYPEUTIL_H_

#include <common/Vector.h>

inline glm::vec3 VecToGLM(glm::dvec3 v) { return glm::vec3(v.x, v.y, v.z); }
inline glm::vec3 VecToGLM(glm::ivec3 v) { return glm::vec3(v.x, v.y, v.z); }

#endif
