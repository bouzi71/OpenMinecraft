#ifndef TERRACOTTA_RENDER_CHUNKMESHGENERATOR_H_
#define TERRACOTTA_RENDER_CHUNKMESHGENERATOR_H_

#include "ChunkMesh.h"

#include <world/World.h>

#include "../PriorityQueue.h"

#include "../assets/AssetCache.h"

#include "../assets/blocks/BlockFace.h"
#include "../assets/blocks/BlockElement.h"
#include "../assets/blocks/BlockModel.h"
#include "../assets/blocks/BlockVariant.h"


struct CMinecraftBlockVertex
{
	glm::vec3 position;
	glm::vec2 uv;
	uint32 texture_index;
	glm::vec3 tint;
	uint32 ambient_occlusion;

	CMinecraftBlockVertex(glm::vec3 pos, glm::vec2 uv, uint32 tex_index, glm::vec3 tint, int ambient_occlusion)
		: position(pos)
		, uv(uv)
		, texture_index(tex_index)
		, tint(tint)
		, ambient_occlusion(static_cast<uint32>(ambient_occlusion))
	{}
};



struct CMinecraftChunkMeshBuildContext
{
	const CMinecraftBlock* chunk_data[18 * 18 * 18];
	glm::ivec3 world_position;

	const CMinecraftBlock* GetBlock(const glm::ivec3& world_pos) const
	{
		glm::ivec3::value_type x = world_pos.x - world_position.x + 1;
		glm::ivec3::value_type y = world_pos.y - world_position.y + 1;
		glm::ivec3::value_type z = world_pos.z - world_position.z + 1;

		return chunk_data[(y * 18 * 18) + (z * 18) + x];
	}
};



class CMinecraftChunkMeshGenerator
	: public WorldListener
{
public:
	using iterator = std::unordered_map<glm::ivec3, std::unique_ptr<CMinecraftChunkMesh>>::iterator;

	CMinecraftChunkMeshGenerator(IRenderDevice& RenderDevice, AssetCache& AssetCache, World* world, std::shared_ptr<ICameraComponent3D> CameraComponent);
	virtual ~CMinecraftChunkMeshGenerator();

	// WorldListener
	void OnBlockChange(glm::ivec3 position, const CMinecraftBlock* newBlock, const CMinecraftBlock* oldBlock) override;
	void OnChunkLoad(ChunkPtr chunk, const ChunkColumnMetadata& meta, uint16 index_y) override;
	void OnChunkUnload(std::shared_ptr<ChunkColumn> chunk) override;

	void GenerateMesh(int64 chunk_x, int64 chunk_y, int64 chunk_z);
	void GenerateMesh(CMinecraftChunkMeshBuildContext& context);

	void DestroyChunk(int64 chunk_x, int64 chunk_y, int64 chunk_z);

	iterator begin() { return m_ChunkMeshes.begin(); }
	iterator end() { return m_ChunkMeshes.end(); }

	void ProcessChunks();

private:
	class ChunkMeshBuildComparator
	{
	public:
		ChunkMeshBuildComparator(std::shared_ptr<ICameraComponent3D> CameraComponent) 
			: m_CameraComponent(CameraComponent)
		{}

		bool operator()(const std::shared_ptr<CMinecraftChunkMeshBuildContext>& first_ctx, const std::shared_ptr<CMinecraftChunkMeshBuildContext>& second_ctx)
		{
			glm::ivec3 first = first_ctx->world_position;
			glm::ivec3 second = second_ctx->world_position;

			glm::vec3 f(first.x, 0, first.z);
			glm::vec3 s(second.x, 0, second.z);

			glm::vec3 a = f - m_CameraComponent->GetPosition();
			glm::vec3 b = s - m_CameraComponent->GetPosition();

			return glm::dot(a, a) > glm::dot(b, b);
		}

	private:
		std::shared_ptr<ICameraComponent3D> m_CameraComponent;
	};

	struct VertexPush
	{
		glm::ivec3 pos;
		std::unique_ptr<std::vector<CMinecraftBlockVertex>> vertices;

		VertexPush(const glm::ivec3& pos, std::unique_ptr<std::vector<CMinecraftBlockVertex>> vertices) 
			: pos(pos)
			, vertices(std::move(vertices)) 
		{}
	};

	bool IsOccluding(const BlockVariant* from_variant, BlockFace face, const CMinecraftBlock* test_block);
	void WorkerUpdate();
	void EnqueueBuildWork(long chunk_x, int chunk_y, long chunk_z);

private:
	IRenderDevice& m_RenderDevice;
	AssetCache& m_AssetCache;

	std::mutex m_QueueMutex;
	PriorityQueue<std::shared_ptr<CMinecraftChunkMeshBuildContext>, ChunkMeshBuildComparator> m_ChunkBuildQueue;
	std::deque<glm::ivec3> m_ChunkPushQueue;
	std::condition_variable m_BuildCV;

	World* m_World;

	std::mutex m_PushMutex;
	std::vector<std::unique_ptr<VertexPush>> m_VertexPushes;

	std::unordered_map<glm::ivec3, std::unique_ptr<CMinecraftChunkMesh>> m_ChunkMeshes;

	bool m_Working;
	std::vector<std::thread> m_Workers;
};

#endif
