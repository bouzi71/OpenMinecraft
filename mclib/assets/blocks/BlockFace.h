#ifndef TERRACOTTA_BLOCK_BLOCKFACE_H_
#define TERRACOTTA_BLOCK_BLOCKFACE_H_



enum class BlockFace
{
	North,
	East,
	South,
	West,


	Up,
	Down,

	None
};

MCLIB_API const std::string& to_string(BlockFace face);
MCLIB_API BlockFace face_from_string(const std::string& str);
MCLIB_API BlockFace get_opposite_face(BlockFace face);

#endif
