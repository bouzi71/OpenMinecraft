#ifndef MCLIB_COMMON_VECTOR_H_
#define MCLIB_COMMON_VECTOR_H_

template <typename T>
inline glm::vec<3, T, glm::defaultp> Vector3RotateAboutY(glm::vec<3, T, glm::defaultp> vec, double rads)
{
	double cosA = std::cos(rads);
	double sinA = std::sin(rads);
	return glm::vec<3, T, glm::defaultp>(cosA * vec.x - sinA * vec.z, vec.y, sinA * vec.x + cosA * vec.z);
}


template <typename T>
inline bool operator<(const glm::vec<3, T, glm::defaultp>& lhs, const glm::vec<3, T, glm::defaultp>& rhs) noexcept
{
	if (lhs.z < rhs.z)
	{
		return true;
	}
	else if (lhs.z > rhs.z)
	{
		return false;
	}

	else if (lhs.y < rhs.y)
	{
		return true;
	}
	else if (lhs.y > rhs.y)
	{
		return false;
	}

	else if (lhs.x < rhs.x)
	{
		return true;
	}

	return false;
}





template <typename T>
inline std::ostream& operator<<(std::ostream& out, const glm::vec<3, T, glm::defaultp>& v)
{
	return out << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

template <typename T>
inline std::wostream& operator<<(std::wostream& out, const glm::vec<3, T, glm::defaultp>& v)
{
	return out << L"(" << v.x << L", " << v.y << L", " << v.z << L")";
}




inline glm::vec3& Truncate(const glm::vec3& V, float length)
{
	glm::vec3 v = V;
	if (glm::length2(v) > length * length)
	{
		v = glm::normalize(v);
		v *= length;
	}
	return v;
}






namespace std
{

template <typename T>
std::string to_string(const glm::vec<3, T, glm::defaultp>& v)
{
	std::stringstream ss;
	ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return ss.str();
}

template <typename T>
struct hash<glm::vec<3, T, glm::defaultp>>
{
	std::size_t operator()(const glm::vec<3, T, glm::defaultp>& type) const
	{
		std::size_t seed = 3;
		for (int i = 0; i < 3; ++i)
		{
			seed ^= type[i] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};
}


#endif
