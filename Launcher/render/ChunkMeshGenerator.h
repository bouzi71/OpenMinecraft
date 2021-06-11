#ifndef TERRACOTTA_RENDER_CHUNKMESHGENERATOR_H_
#define TERRACOTTA_RENDER_CHUNKMESHGENERATOR_H_

#include "ChunkMesh.h"

#include <utility>


#include <common/Vector.h>
#include <world/World.h>

#include "../PriorityQueue.h"

#include <assets/blocks/BlockFace.h>
#include <assets/blocks/BlockElement.h>
#include <assets/blocks/BlockModel.h>
#include <assets/blocks/BlockVariant.h>

#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>


namespace terra
{

namespace block
{
class BlockModel;
} // ns block

namespace render
{

struct Vertex
{
	glm::vec3 position;
	glm::vec2 uv;
	uint32 texture_index;
	glm::vec3 tint;
	uint32 ambient_occlusion;

	Vertex(glm::vec3 pos, glm::vec2 uv, uint32 tex_index, glm::vec3 tint, int ambient_occlusion)
		: position(pos)
		, uv(uv)
		, texture_index(tex_index)
		, tint(tint)
		, ambient_occlusion(static_cast<uint32>(ambient_occlusion))
	{}
};

struct ChunkMeshBuildContext
{
	const CMinecraftBlock* chunk_data[18 * 18 * 18];
	glm::ivec3 world_position;

	const CMinecraftBlock* GetBlock(const glm::ivec3& world_pos)
	{
		glm::ivec3::value_type x = world_pos.x - world_position.x + 1;
		glm::ivec3::value_type y = world_pos.y - world_position.y + 1;
		glm::ivec3::value_type z = world_pos.z - world_position.z + 1;

		return chunk_data[(y * 18 * 18) + (z * 18) + x];
	}
};

class ChunkMeshGenerator
	: public WorldListener
{
public:
	using iterator = std::unordered_map<glm::ivec3, std::unique_ptr<terra::render::ChunkMesh>>::iterator;

	ChunkMeshGenerator(IRenderDevice& RenderDevice, World* world, const glm::vec3& camera_position);
	virtual ~ChunkMeshGenerator();

	// WorldListener
	void OnBlockChange(glm::ivec3 position, const CMinecraftBlock* newBlock, const CMinecraftBlock* oldBlock) override;
	void OnChunkLoad(ChunkPtr chunk, const ChunkColumnMetadata& meta, uint16 index_y) override;
	void OnChunkUnload(std::shared_ptr<ChunkColumn> chunk) override;

	void GenerateMesh(int64 chunk_x, int64 chunk_y, int64 chunk_z);
	void GenerateMesh(ChunkMeshBuildContext& context);
	void DestroyChunk(int64 chunk_x, int64 chunk_y, int64 chunk_z);

	iterator begin() { return m_ChunkMeshes.begin(); }
	iterator end() { return m_ChunkMeshes.end(); }

	void ProcessChunks();

private:
	struct ChunkMeshBuildComparator
	{
		const glm::vec3& position;

		ChunkMeshBuildComparator(const glm::vec3& position) 
			: position(position) 
		{}

		bool operator()(const std::shared_ptr<ChunkMeshBuildContext>& first_ctx, const std::shared_ptr<ChunkMeshBuildContext>& second_ctx)
		{
			glm::ivec3 first = first_ctx->world_position;
			glm::ivec3 second = second_ctx->world_position;

			glm::vec3 f(first.x, 0, first.z);
			glm::vec3 s(second.x, 0, second.z);

			glm::vec3 a = f - position;
			glm::vec3 b = s - position;

			return glm::dot(a, a) > glm::dot(b, b);
		}
	};

	struct VertexPush
	{
		glm::ivec3 pos;
		std::unique_ptr<std::vector<Vertex>> vertices;

		VertexPush(const glm::ivec3& pos, std::unique_ptr<std::vector<Vertex>> vertices) 
			: pos(pos)
			, vertices(std::move(vertices)) 
		{}
	};

	int GetAmbientOcclusion(ChunkMeshBuildContext& context, const glm::ivec3& side1, const glm::ivec3& side2, const glm::ivec3& corner);
	bool IsOccluding(BlockVariant* from_variant, BlockFace face, const CMinecraftBlock* test_block);
	void WorkerUpdate();
	void EnqueueBuildWork(long chunk_x, int chunk_y, long chunk_z);

private:
	IRenderDevice& m_RenderDevice;

	std::mutex m_QueueMutex;
	PriorityQueue<std::shared_ptr<ChunkMeshBuildContext>, ChunkMeshBuildComparator> m_ChunkBuildQueue;
	std::deque<glm::ivec3> m_ChunkPushQueue;
	std::condition_variable m_BuildCV;

	World* m_World;

	std::mutex m_PushMutex;
	std::vector<std::unique_ptr<VertexPush>> m_VertexPushes;

	std::unordered_map<glm::ivec3, std::unique_ptr<terra::render::ChunkMesh>> m_ChunkMeshes;

	bool m_Working;
	std::vector<std::thread> m_Workers;
};

} // ns render
} // ns terra

#endif
