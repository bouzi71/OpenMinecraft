#include "stdafx.h"

// General
#include "ChunkMeshGenerator.h"

// Additional
#include "../assets/AssetCache.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace
{
struct SFacePlusVertex
{
	SFacePlusVertex(const glm::vec3& Pos, BlockFace Face)
		: Pos(Pos)
		, Face(Face)
	{}

	glm::vec3 Pos;
	BlockFace Face;
} verticesPlusFaces[] = 
{
	SFacePlusVertex(glm::vec3(0,  1, 0), BlockFace::Up),
	SFacePlusVertex(glm::vec3(0, -1, 0), BlockFace::Down),

	SFacePlusVertex(glm::vec3(0, 0,  1), BlockFace::South),
	SFacePlusVertex(glm::vec3(0, 0, -1), BlockFace::North),

	SFacePlusVertex(glm::vec3(1,  0, 0), BlockFace::East),
	SFacePlusVertex(glm::vec3(-1, 0, 0), BlockFace::West),
};



}


CMinecraftChunkMeshGenerator::CMinecraftChunkMeshGenerator(IRenderDevice& RenderDevice, AssetCache& AssetCache, World* world, std::shared_ptr<ICameraComponent3D> CameraComponent)
	: m_RenderDevice(RenderDevice)
	, m_AssetCache(AssetCache)
	, m_World(world)
	, m_ChunkBuildQueue(ChunkMeshBuildComparator(CameraComponent))
{
	m_World->RegisterListener(this);

	m_Working = true;
	unsigned int count = std::thread::hardware_concurrency() - 1;

	Log::Print("Creating %d mesh worker threads.", count);

	for (unsigned int i = 0; i < count; ++i)
	{
		m_Workers.emplace_back(&CMinecraftChunkMeshGenerator::WorkerUpdate, this);
	}
}

CMinecraftChunkMeshGenerator::~CMinecraftChunkMeshGenerator()
{
	m_Working = false;

	m_BuildCV.notify_all();

	for (auto& worker : m_Workers)
		worker.join();

	m_World->UnregisterListener(this);
}

void CMinecraftChunkMeshGenerator::OnBlockChange(glm::ivec3 position, const CMinecraftBlock * newBlock, const CMinecraftBlock * oldBlock)
{
	std::shared_ptr<ChunkColumn> column = m_World->GetChunk(position);

	const glm::ivec3 chunk_base(column->GetMetadata().x * 16, (position.y / 16) * 16, column->GetMetadata().z * 16);

	int chunk_x = column->GetMetadata().x;
	int chunk_y = static_cast<int>(position.y / 16);
	int chunk_z = column->GetMetadata().z;

	auto iter = m_ChunkMeshes.find(chunk_base);
	if (iter == m_ChunkMeshes.end())
	{
		GenerateMesh(chunk_x, chunk_y, chunk_z);
		return;
	}


	EnqueueBuildWork(chunk_x, chunk_y, chunk_z);


	if (position.x % 16 == 0)
	{
		EnqueueBuildWork(chunk_x - 1, chunk_y, chunk_z);
	}
	else if (position.x % 16 == 15)
	{
		EnqueueBuildWork(chunk_x + 1, chunk_y, chunk_z);
	}


	if (position.y % 16 == 0)
	{
		EnqueueBuildWork(chunk_x, chunk_y - 1, chunk_z);
	}
	else if (position.y % 16 == 15)
	{
		EnqueueBuildWork(chunk_x, chunk_y + 1, chunk_z);
	}


	if (position.z % 16 == 0)
	{
		EnqueueBuildWork(chunk_x, chunk_y, chunk_z - 1);
	}
	else if (position.z % 16 == 15)
	{
		EnqueueBuildWork(chunk_x, chunk_y, chunk_z + 1);
	}
}

void CMinecraftChunkMeshGenerator::OnChunkLoad(ChunkPtr chunk, const ChunkColumnMetadata& meta, uint16 index_y)
{
	EnqueueBuildWork(meta.x, index_y, meta.z);

	EnqueueBuildWork(meta.x - 1, index_y, meta.z);
	EnqueueBuildWork(meta.x + 1, index_y, meta.z);
	EnqueueBuildWork(meta.x, index_y - 1, meta.z);
	EnqueueBuildWork(meta.x, index_y + 1, meta.z);
	EnqueueBuildWork(meta.x, index_y, meta.z - 1);
	EnqueueBuildWork(meta.x, index_y, meta.z + 1);
}

void CMinecraftChunkMeshGenerator::EnqueueBuildWork(long chunk_x, int chunk_y, long chunk_z)
{
	glm::ivec3 position(chunk_x * 16, chunk_y * 16, chunk_z * 16);

	auto iter = std::find(m_ChunkPushQueue.begin(), m_ChunkPushQueue.end(), position);
	if (iter == m_ChunkPushQueue.end())
	{
		m_ChunkPushQueue.push_back(position);
	}
}

void CMinecraftChunkMeshGenerator::WorkerUpdate()
{
	while (m_Working)
	{
		if (m_ChunkBuildQueue.Empty())
		{
			std::unique_lock<std::mutex> lock(m_QueueMutex);
			m_BuildCV.wait(lock);
		}

		std::shared_ptr<CMinecraftChunkMeshBuildContext> ctx;

		{
			std::lock_guard<std::mutex> lock(m_QueueMutex);
			if (m_ChunkBuildQueue.Empty()) 
				continue;

			ctx = m_ChunkBuildQueue.Pop();
		}

		GenerateMesh(*ctx);
	}
}

void CMinecraftChunkMeshGenerator::ProcessChunks()
{
	const std::size_t kMaxMeshesPerFrame = 64;

	// Push any new chunks that were added this frame into the work queue
	for (std::size_t i = 0; i < kMaxMeshesPerFrame && false == m_ChunkPushQueue.empty(); ++i)
	{
		glm::ivec3 chunk_base = m_ChunkPushQueue.front();
		m_ChunkPushQueue.pop_front();

		auto ctx = std::make_shared<CMinecraftChunkMeshBuildContext>();

		ctx->world_position = chunk_base;
		glm::ivec3 offset_y(0, chunk_base.y, 0);

		std::shared_ptr<ChunkColumn> columns[3][3];

		columns[1][1] = m_World->GetChunk(chunk_base);

		// Cache the world data for this chunk so it can be pushed to another thread and built.
		for (int64 y = 0; y < 18; ++y)
		{
			for (int64 z = 0; z < 18; ++z)
			{
				for (int64 x = 0; x < 18; ++x)
				{
					glm::ivec3 offset(x - 1, y - 1, z - 1);
					const CMinecraftBlock* block = nullptr;

					std::size_t x_index = (int64)std::floor(offset.x / 16.0) + 1;
					std::size_t z_index = (int64)std::floor(offset.z / 16.0) + 1;
					auto& column = columns[x_index][z_index];

					if (column == nullptr)
					{
						column = m_World->GetChunk(chunk_base + offset);
					}

					if (column != nullptr)
					{
						glm::ivec3 lookup = offset + offset_y;

						if (lookup.x < 0)
						{
							lookup.x += 16;
						}
						else if (lookup.x > 15)
						{
							lookup.x -= 16;
						}

						if (lookup.z < 0)
						{
							lookup.z += 16;
						}
						else if (lookup.z > 15)
						{
							lookup.z -= 16;
						}

						block = column->GetBlock(lookup);
					}

					ctx->chunk_data[(y * 18 * 18) + (z * 18) + x] = block;
				}
			}
		}

		{
			std::lock_guard<std::mutex> lock(m_QueueMutex);
			m_ChunkBuildQueue.Push(ctx);
		}

		m_BuildCV.notify_one();
	}

	std::vector<std::unique_ptr<VertexPush>> pushes;

	{
		std::lock_guard<std::mutex> lock(m_PushMutex);
		pushes = std::move(m_VertexPushes);
		m_VertexPushes = std::vector<std::unique_ptr<VertexPush>>();
	}

	for (auto&& push : pushes)
	{
		DestroyChunk(push->pos.x / 16, push->pos.y / 16, push->pos.z / 16);

		auto&& vertices = push->vertices;
		if (vertices->empty()) 
			continue;

		std::shared_ptr<IBuffer> verticesB = m_RenderDevice.GetObjectsFactory().CreateVertexBuffer(*vertices);

		std::unique_ptr<CMinecraftChunkMesh> mesh = std::make_unique<CMinecraftChunkMesh>(m_RenderDevice, verticesB);
		m_ChunkMeshes[push->pos] = std::move(mesh);
	}
}



bool CMinecraftChunkMeshGenerator::IsOccluding(const BlockVariant* from_variant, BlockFace face, const CMinecraftBlock* test_block)
{
	if (test_block == nullptr) 
		return false;

	//if (from_variant->HasRotation()) 
	//	return false;

	const BlockModel* fromVariantModel = from_variant->GetModel();

	for (const auto& element : from_variant->GetModel()->GetElements())
	{
		if (element.GetFace(face).cull_face == BlockFace::None)
		{
			return false;
		}
	}

	const BlockVariant* variant = m_AssetCache.GetBlockstatesLoader().GetVariant(test_block);
	if (variant == nullptr) 
		return false;

	//if (variant->HasRotation())
	//	return false;

	const BlockModel* model = variant->GetModel();
	if (model == nullptr)
		return false;

	bool is_full = false;

	BlockFace opposite = get_opposite_face(face);
	const auto& textures = m_AssetCache.GetTexturesLoader();
	for (auto& element : model->GetElements())
	{
		auto opposite_face = element.GetFace(opposite);
		bool transparent = textures.IsTransparent(opposite_face.texture);
		bool full_extent = element.IsFullExtent();

		if (full_extent && (false == transparent))
		{
			is_full = true;
		}
	}

	return is_full;
}

void ApplyRotations(glm::vec3& bottom_left, glm::vec3& bottom_right, glm::vec3& top_left, glm::vec3& top_right, const glm::vec3& rotations, glm::vec3 offset = glm::vec3(0.5, 0.5, 0.5))
{
	glm::quat quat(1.0f, 0.0f, 0.0f, 0.0f);

	if (rotations.z != 0)
		quat = glm::rotate(quat, glm::radians( rotations.z), glm::vec3(0.0f, 0.0f, 1.0f));

	if (rotations.y != 0)
		quat = glm::rotate(quat, glm::radians(-rotations.y), glm::vec3(0.0f, 1.0f, 0.0f));

	if (rotations.x != 0)
		quat = glm::rotate(quat, glm::radians( rotations.x), glm::vec3(1.0f, 0.0f, 0.0f));

	if (rotations.x != 0 || rotations.y != 0 || rotations.z != 0)
	{
		bottom_left  = glm::vec3(quat * glm::vec4(bottom_left  - offset, 1.0)) + offset;
		bottom_right = glm::vec3(quat * glm::vec4(bottom_right - offset, 1.0)) + offset;
		top_left     = glm::vec3(quat * glm::vec4(top_left     - offset, 1.0)) + offset;
		top_right    = glm::vec3(quat * glm::vec4(top_right    - offset, 1.0)) + offset;
	}
}

// TODO: Rescaling?
void ApplyRotations(glm::vec3& bottom_left, glm::vec3& bottom_right, glm::vec3& top_left, glm::vec3& top_right, const glm::vec3& variant_rotation, const ElementRotation& rotation)
{
	glm::vec3 rotations(rotation.angle, rotation.angle, rotation.angle);

	glm::quat quat(1, 0, 0, 0);

	const float kToRads = (float)M_PI / 180.0f;

	if (variant_rotation.z != 0)
	{
		quat = glm::rotate(quat, kToRads * variant_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	if (variant_rotation.y != 0)
	{
		quat = glm::rotate(quat, kToRads * -variant_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	if (variant_rotation.x != 0)
	{
		quat = glm::rotate(quat, kToRads * variant_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	if (rotation.rescale)
	{
		rotations = rotations * (1.0f / std::cos(3.14159f / 4.0f) - 1.0f);
	}

	// Hadamard product to get just the one axis rotation
	rotations = rotations * (quat * rotation.axis);

	ApplyRotations(bottom_left, bottom_right, top_left, top_right, rotations, quat * (rotation.origin - glm::vec3(0.5, 0.5, 0.5)) + glm::vec3(0.5, 0.5, 0.5));
}






void RotateCube(std::vector<SFacePlusVertex>& Vector, const glm::vec3& Rotations)
{
	Vector.clear();

	Vector.push_back(SFacePlusVertex(glm::vec3( 1, 0, 0), BlockFace::East));
	Vector.push_back(SFacePlusVertex(glm::vec3(-1, 0, 0), BlockFace::West));

	Vector.push_back(SFacePlusVertex(glm::vec3(0,  1, 0), BlockFace::Up));
	Vector.push_back(SFacePlusVertex(glm::vec3(0, -1, 0), BlockFace::Down));

	Vector.push_back(SFacePlusVertex(glm::vec3(0, 0,  1), BlockFace::South));
	Vector.push_back(SFacePlusVertex(glm::vec3(0, 0, -1), BlockFace::North));


	glm::quat quat(1.0f, 0.0f, 0.0f, 0.0f);

	if (Rotations.z != 0)
		quat = glm::rotate(quat, glm::radians(Rotations.z), glm::vec3(0.0f, 0.0f, 1.0f));

	if (Rotations.y != 0)
		quat = glm::rotate(quat, glm::radians(-Rotations.y), glm::vec3(0.0f, 1.0f, 0.0f));

	if (Rotations.x != 0)
		quat = glm::rotate(quat, glm::radians(Rotations.x), glm::vec3(1.0f, 0.0f, 0.0f));

	for (auto& it : Vector)
	{
		it.Pos = glm::vec3(quat * glm::vec4(it.Pos, 1.0f));
	}
}

BlockFace GetFaceFromCoord(std::vector<SFacePlusVertex>& Vector, const glm::vec3& Coord)
{
	if (Vector.empty())
		throw CException("Incorrect usage!");

	const auto& faceIt = std::find_if(Vector.begin(), Vector.end(), [&Coord](const SFacePlusVertex& _Item) -> bool{
		return glm::abs(_Item.Pos.x - Coord.x) < 0.1f && glm::abs(_Item.Pos.y - Coord.y) < 0.1f && glm::abs(_Item.Pos.z - Coord.z) < 0.1f;
	});
	if (faceIt == Vector.end())
		throw CException("Incorrect usage!");


	return faceIt->Face;
}

BlockFace GetFaceFromRotation(std::vector<SFacePlusVertex>& Vector, BlockFace Face)
{
	if (Face == BlockFace::North)
	{
		return GetFaceFromCoord(Vector, glm::vec3(0, 0, -1));
	}
	else if (Face == BlockFace::South)
	{
		return GetFaceFromCoord(Vector, glm::vec3(0, 0, 1));
	}
	else if (Face == BlockFace::East)
	{
		return GetFaceFromCoord(Vector, glm::vec3(1, 0, 0));
	}
	else if (Face == BlockFace::West)
	{
		return GetFaceFromCoord(Vector, glm::vec3(-1, 0, 0));
	}
	else if (Face == BlockFace::Up)
	{
		return GetFaceFromCoord(Vector, glm::vec3(0, 1, 0));
	}
	else if (Face == BlockFace::Down)
	{
		return GetFaceFromCoord(Vector, glm::vec3(0, -1, 0));
	}

	_ASSERT(false);
	return BlockFace::None;
}


int GetAmbientOcclusion(const CMinecraftChunkMeshBuildContext& context, const glm::ivec3& side1, const glm::ivec3& side2, const glm::ivec3& corner)
{
	int value1, value2, value_corner;

	auto bs1 = context.GetBlock(side1);
	auto bs2 = context.GetBlock(side2);
	auto bsc = context.GetBlock(corner);

	value1 = bs1 && bs1->IsSolid();
	value2 = bs2 && bs2->IsSolid();
	value_corner = bsc && bsc->IsSolid();

	if (value1 && value2)
	{
		return 0;
	}

	return 3 - (value1 + value2 + value_corner);
}

void FillVerticesForSide(std::vector<CMinecraftBlockVertex>& Vertices, const CMinecraftChunkMeshBuildContext& context, const glm::ivec3& BlockPosition, const BlockVariant * BlockVariant, const BlockModel * BlockModel, BlockFace Face)
{
	static const glm::vec3 kTints[] = {
		glm::vec3(1.0, 1.0, 1.0),
		glm::vec3(137 / 255.0, 191 / 255.0, 98 / 255.0), // Grass
		glm::vec3(0.22, 0.60, 0.21), // Leaves
	};


	if (Face == BlockFace::Up)
	{
		// Render the top face of the current block.
		int obl = 3, obr = 3, otl = 3, otr = 3;

		if (false == BlockVariant->HasRotation())
		{
			obl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, 1, 0), BlockPosition + glm::ivec3(0, 1, -1), BlockPosition + glm::ivec3(-1, 1, -1));
			obr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, 1, 0), BlockPosition + glm::ivec3(0, 1, 1), BlockPosition + glm::ivec3(-1, 1, 1));
			otl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, 1, 0), BlockPosition + glm::ivec3(0, 1, -1), BlockPosition + glm::ivec3(1, 1, -1));
			otr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, 1, 0), BlockPosition + glm::ivec3(0, 1, 1), BlockPosition + glm::ivec3(1, 1, 1));
		}

		for (const auto& element : BlockModel->GetElements())
		{
			RenderableFace renderable = element.GetFace(Face);
			if (renderable.face == Face)
			{
				TextureHandle texture = renderable.texture;

				const auto& from = element.GetFrom();
				const auto& to = element.GetTo();

				glm::vec3 bottom_left = glm::vec3(from.x, to.y, from.z);
				glm::vec3 bottom_right = glm::vec3(from.x, to.y, to.z);
				glm::vec3 top_left = glm::vec3(to.x, to.y, from.z);
				glm::vec3 top_right = glm::vec3(to.x, to.y, to.z);

				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations());
				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations(), element.GetRotation());

				bottom_left += BlockPosition;
				bottom_right += BlockPosition;
				top_left += BlockPosition;
				top_right += BlockPosition;

				const glm::vec3& tint = kTints[renderable.tint_index + 1];

				glm::vec2 bl_uv(renderable.uv_from.x, renderable.uv_from.y);
				glm::vec2 br_uv(renderable.uv_from.x, renderable.uv_to.y);
				glm::vec2 tr_uv(renderable.uv_to.x, renderable.uv_to.y);
				glm::vec2 tl_uv(renderable.uv_to.x, renderable.uv_from.y);

				int cobl = 3, cobr = 3, cotl = 3, cotr = 3;
				if (element.ShouldShade())
				{
					cobl = obl;
					cobr = obr;
					cotl = otl;
					cotr = otr;
				}

				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
				Vertices.emplace_back(bottom_right, br_uv, texture, tint, cobr);
				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);

				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);
				Vertices.emplace_back(top_left, tl_uv, texture, tint, cotl);
				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
			}
		}
	}
	else if (Face == BlockFace::Down)
	{
		// Render the bottom face of the current block.
		int obl = 3, obr = 3, otl = 3, otr = 3;

		if (false == BlockVariant->HasRotation())
		{
			obl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, -1, 0), BlockPosition + glm::ivec3(0, -1, -1), BlockPosition + glm::ivec3(1, -1, -1));
			obr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, -1, 0), BlockPosition + glm::ivec3(0, -1, 1), BlockPosition + glm::ivec3(1, -1, 1));
			otl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, -1, 0), BlockPosition + glm::ivec3(0, -1, -1), BlockPosition + glm::ivec3(-1, -1, -1));
			otr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, -1, 0), BlockPosition + glm::ivec3(0, -1, 1), BlockPosition + glm::ivec3(-1, -1, 1));
		}

		for (const auto& element : BlockModel->GetElements())
		{
			RenderableFace renderable = element.GetFace(Face);
			if (renderable.face == Face)
			{
				const auto& from = element.GetFrom();
				const auto& to = element.GetTo();

				glm::vec3 bottom_left = glm::vec3(to.x, from.y, from.z);
				glm::vec3 bottom_right = glm::vec3(to.x, from.y, to.z);
				glm::vec3 top_left = glm::vec3(from.x, from.y, from.z);
				glm::vec3 top_right = glm::vec3(from.x, from.y, to.z);

				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations());
				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations(), element.GetRotation());

				bottom_left += BlockPosition;
				bottom_right += BlockPosition;
				top_left += BlockPosition;
				top_right += BlockPosition;

				const glm::vec3& tint = kTints[renderable.tint_index + 1];

				glm::vec2 bl_uv(renderable.uv_to.x, renderable.uv_to.y);
				glm::vec2 br_uv(renderable.uv_to.x, renderable.uv_from.y);
				glm::vec2 tr_uv(renderable.uv_from.x, renderable.uv_from.y);
				glm::vec2 tl_uv(renderable.uv_from.x, renderable.uv_to.y);

				int cobl = 3, cobr = 3, cotl = 3, cotr = 3;
				if (element.ShouldShade())
				{
					cobl = obl;
					cobr = obr;
					cotl = otl;
					cotr = otr;
				}

				TextureHandle texture = renderable.texture;

				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
				Vertices.emplace_back(bottom_right, br_uv, texture, tint, cobr);
				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);

				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);
				Vertices.emplace_back(top_left, tl_uv, texture, tint, cotl);
				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
			}
		}
	}
	else if (Face == BlockFace::North)
	{
		// Render the north face of the current block.
		int obl = 3, obr = 3, otl = 3, otr = 3;

		if (false == BlockVariant->HasRotation())
		{
			obl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, 0, -1), BlockPosition + glm::ivec3(0, -1, -1), BlockPosition + glm::ivec3(1, -1, -1));
			obr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(0, -1, -1), BlockPosition + glm::ivec3(-1, 0, -1), BlockPosition + glm::ivec3(-1, -1, -1));
			otl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(0, 1, -1), BlockPosition + glm::ivec3(1, 0, -1), BlockPosition + glm::ivec3(1, 1, -1));
			otr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(0, 1, -1), BlockPosition + glm::ivec3(-1, 0, -1), BlockPosition + glm::ivec3(-1, 1, -1));
		}

		for (const auto& element : BlockModel->GetElements())
		{
			RenderableFace renderable = element.GetFace(Face);
			if (renderable.face == Face)
			{
				TextureHandle texture = renderable.texture;

				const auto& from = element.GetFrom();
				const auto& to = element.GetTo();

				glm::vec3 bottom_left = glm::vec3(to.x, from.y, from.z);
				glm::vec3 bottom_right = glm::vec3(from.x, from.y, from.z);
				glm::vec3 top_left = glm::vec3(to.x, to.y, from.z);
				glm::vec3 top_right = glm::vec3(from.x, to.y, from.z);

				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations());
				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations(), element.GetRotation());

				bottom_left += BlockPosition;
				bottom_right += BlockPosition;
				top_left += BlockPosition;
				top_right += BlockPosition;

				const glm::vec3& tint = kTints[renderable.tint_index + 1];

				glm::vec2 bl_uv(renderable.uv_from.x, renderable.uv_to.y);
				glm::vec2 br_uv(renderable.uv_to.x, renderable.uv_to.y);
				glm::vec2 tr_uv(renderable.uv_to.x, renderable.uv_from.y);
				glm::vec2 tl_uv(renderable.uv_from.x, renderable.uv_from.y);

				int cobl = 3, cobr = 3, cotl = 3, cotr = 3;
				if (element.ShouldShade())
				{
					cobl = obl;
					cobr = obr;
					cotl = otl;
					cotr = otr;
				}

				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
				Vertices.emplace_back(bottom_right, br_uv, texture, tint, cobr);
				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);

				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);
				Vertices.emplace_back(top_left, tl_uv, texture, tint, cotl);
				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
			}
		}
	}
	else if (Face == BlockFace::South)
	{
		// Render the south face of the current block.
		int obl = 3, obr = 3, otl = 3, otr = 3;

		if (false == BlockVariant->HasRotation())
		{
			obl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, 0, 1), BlockPosition + glm::ivec3(0, -1, 1), BlockPosition + glm::ivec3(-1, -1, 1));
			obr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, 0, 1), BlockPosition + glm::ivec3(0, -1, 1), BlockPosition + glm::ivec3(1, -1, 1));
			otl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(0, 1, 1), BlockPosition + glm::ivec3(-1, 0, 1), BlockPosition + glm::ivec3(-1, 1, 1));
			otr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(0, 1, 1), BlockPosition + glm::ivec3(1, 0, 1), BlockPosition + glm::ivec3(1, 1, 1));
		}

		for (const auto& element : BlockModel->GetElements())
		{
			RenderableFace renderable = element.GetFace(Face);
			if (renderable.face == Face)
			{
				TextureHandle texture = renderable.texture;

				const auto& from = element.GetFrom();
				const auto& to = element.GetTo();

				glm::vec3 bottom_left = glm::vec3(from.x, from.y, to.z);
				glm::vec3 bottom_right = glm::vec3(to.x, from.y, to.z);
				glm::vec3 top_left = glm::vec3(from.x, to.y, to.z);
				glm::vec3 top_right = glm::vec3(to.x, to.y, to.z);

				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations());
				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations(), element.GetRotation());

				bottom_left += BlockPosition;
				bottom_right += BlockPosition;
				top_left += BlockPosition;
				top_right += BlockPosition;

				const glm::vec3& tint = kTints[renderable.tint_index + 1];

				glm::vec2 bl_uv(renderable.uv_from.x, renderable.uv_to.y);
				glm::vec2 br_uv(renderable.uv_to.x, renderable.uv_to.y);
				glm::vec2 tr_uv(renderable.uv_to.x, renderable.uv_from.y);
				glm::vec2 tl_uv(renderable.uv_from.x, renderable.uv_from.y);

				int cobl = 3, cobr = 3, cotl = 3, cotr = 3;
				if (element.ShouldShade())
				{
					cobl = obl;
					cobr = obr;
					cotl = otl;
					cotr = otr;
				}

				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
				Vertices.emplace_back(bottom_right, br_uv, texture, tint, cobr);
				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);

				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);
				Vertices.emplace_back(top_left, tl_uv, texture, tint, cotl);
				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
			}
		}
	}
	else if (Face == BlockFace::East)
	{
		// Render the east face of the current block.
		int obl = 3, obr = 3, otl = 3, otr = 3;

		if (false == BlockVariant->HasRotation())
		{
			obl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, 0, 1), BlockPosition + glm::ivec3(1, -1, 0), BlockPosition + glm::ivec3(1, -1, 1));
			obr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, -1, 0), BlockPosition + glm::ivec3(1, 0, -1), BlockPosition + glm::ivec3(1, -1, -1));
			otl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, 1, 0), BlockPosition + glm::ivec3(1, 0, 1), BlockPosition + glm::ivec3(1, 1, 1));
			otr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(1, 1, 0), BlockPosition + glm::ivec3(1, 0, -1), BlockPosition + glm::ivec3(1, 1, -1));
		}

		for (const auto& element : BlockModel->GetElements())
		{
			RenderableFace renderable = element.GetFace(Face);
			if (renderable.face == Face)
			{
				TextureHandle texture = renderable.texture;

				const auto& from = element.GetFrom();
				const auto& to = element.GetTo();

				glm::vec3 bottom_left = glm::vec3(to.x, from.y, to.z);
				glm::vec3 bottom_right = glm::vec3(to.x, from.y, from.z);
				glm::vec3 top_left = glm::vec3(to.x, to.y, to.z);
				glm::vec3 top_right = glm::vec3(to.x, to.y, from.z);

				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations());
				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations(), element.GetRotation());

				bottom_left += BlockPosition;
				bottom_right += BlockPosition;
				top_left += BlockPosition;
				top_right += BlockPosition;

				const glm::vec3& tint = kTints[renderable.tint_index + 1];

				glm::vec2 bl_uv(renderable.uv_from.x, renderable.uv_to.y);
				glm::vec2 br_uv(renderable.uv_to.x, renderable.uv_to.y);
				glm::vec2 tr_uv(renderable.uv_to.x, renderable.uv_from.y);
				glm::vec2 tl_uv(renderable.uv_from.x, renderable.uv_from.y);

				int cobl = 3, cobr = 3, cotl = 3, cotr = 3;
				if (element.ShouldShade())
				{
					cobl = obl;
					cobr = obr;
					cotl = otl;
					cotr = otr;
				}

				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
				Vertices.emplace_back(bottom_right, br_uv, texture, tint, cobr);
				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);

				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);
				Vertices.emplace_back(top_left, tl_uv, texture, tint, cotl);
				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
			}
		}
	}
	else if (Face == BlockFace::West)
	{
		// Render the west face of the current block.
		int obl = 3, obr = 3, otl = 3, otr = 3;

		if (false == BlockVariant->HasRotation())
		{
			obl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, -1, 0), BlockPosition + glm::ivec3(-1, 0, -1), BlockPosition + glm::ivec3(-1, -1, -1));
			obr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, -1, 0), BlockPosition + glm::ivec3(-1, 0, 1), BlockPosition + glm::ivec3(-1, -1, 1));
			otl = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, 1, 0), BlockPosition + glm::ivec3(-1, 0, -1), BlockPosition + glm::ivec3(-1, 1, -1));
			otr = GetAmbientOcclusion(context, BlockPosition + glm::ivec3(-1, 1, 0), BlockPosition + glm::ivec3(-1, 0, 1), BlockPosition + glm::ivec3(-1, 1, 1));
		}

		for (const auto& element : BlockModel->GetElements())
		{
			RenderableFace renderable = element.GetFace(Face);
			if (renderable.face == Face)
			{
				TextureHandle texture = renderable.texture;

				const auto& from = element.GetFrom();
				const auto& to = element.GetTo();

				glm::vec3 bottom_left = glm::vec3(from.x, from.y, from.z);
				glm::vec3 bottom_right = glm::vec3(from.x, from.y, to.z);
				glm::vec3 top_left = glm::vec3(from.x, to.y, from.z);
				glm::vec3 top_right = glm::vec3(from.x, to.y, to.z);

				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations());
				ApplyRotations(bottom_left, bottom_right, top_left, top_right, BlockVariant->GetRotations(), element.GetRotation());

				bottom_left += BlockPosition;
				bottom_right += BlockPosition;
				top_left += BlockPosition;
				top_right += BlockPosition;

				const glm::vec3& tint = kTints[renderable.tint_index + 1];

				glm::vec2 bl_uv(renderable.uv_from.x, renderable.uv_to.y);
				glm::vec2 br_uv(renderable.uv_to.x, renderable.uv_to.y);
				glm::vec2 tr_uv(renderable.uv_to.x, renderable.uv_from.y);
				glm::vec2 tl_uv(renderable.uv_from.x, renderable.uv_from.y);

				int cobl = 3, cobr = 3, cotl = 3, cotr = 3;
				if (element.ShouldShade())
				{
					cobl = obl;
					cobr = obr;
					cotl = otl;
					cotr = otr;
				}

				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
				Vertices.emplace_back(bottom_right, br_uv, texture, tint, cobr);
				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);

				Vertices.emplace_back(top_right, tr_uv, texture, tint, cotr);
				Vertices.emplace_back(top_left, tl_uv, texture, tint, cotl);
				Vertices.emplace_back(bottom_left, bl_uv, texture, tint, cobl);
			}
		}
		
	}
}





// TODO: Calculate occlusion under rotation
// TODO: Calculate UV under rotation for UV locked variants
void CMinecraftChunkMeshGenerator::GenerateMesh(CMinecraftChunkMeshBuildContext& context)
{
	std::unique_ptr<std::vector<CMinecraftBlockVertex>> vertices = std::make_unique<std::vector<CMinecraftBlockVertex>>();
	vertices->reserve(12500);

	// Sweep through the blocks and generate vertices for the mesh
	for (int y = 0; y < 16; ++y)
	{
		for (int z = 0; z < 16; ++z)
		{
			for (int x = 0; x < 16; ++x)
			{
				glm::ivec3 BlockPosition = context.world_position + glm::ivec3(x, y, z);

				const CMinecraftBlock* currentBlock = context.GetBlock(BlockPosition);
				if (currentBlock == nullptr)
					continue;

				const BlockVariant* currentVariant = m_AssetCache.GetBlockstatesLoader().GetVariant(currentBlock);
				if (currentVariant == nullptr)
					continue;

				const BlockModel* currentVariantModel = currentVariant->GetModel();
				if (currentVariantModel == nullptr)
					continue;

				if (currentVariantModel->GetElements().empty())
					continue;

				std::vector<SFacePlusVertex> vertices2;
				RotateCube(vertices2, currentVariant->GetRotations());

				for (size_t i = 0; i < sizeof(verticesPlusFaces) / sizeof(SFacePlusVertex); i++)
				{
					const SFacePlusVertex& facePlusVertex = verticesPlusFaces[i];

					const CMinecraftBlock* block = context.GetBlock(BlockPosition + glm::ivec3(facePlusVertex.Pos));
					if (false == IsOccluding(currentVariant, facePlusVertex.Face, block))
					{
						const BlockFace aboveFace = GetFaceFromRotation(vertices2, facePlusVertex.Face);
						FillVerticesForSide(*vertices, context, BlockPosition, currentVariant, currentVariantModel, aboveFace);
					}
				}
			}
		}
	}

	std::unique_ptr<VertexPush> push = std::make_unique<VertexPush>(context.world_position, std::move(vertices));

	std::lock_guard<std::mutex> lock(m_PushMutex);
	m_VertexPushes.push_back(std::move(push));
}

void CMinecraftChunkMeshGenerator::GenerateMesh(int64 chunk_x, int64 chunk_y, int64 chunk_z)
{
	CMinecraftChunkMeshBuildContext ctx;
	ctx.world_position = glm::ivec3(chunk_x * 16, chunk_y * 16, chunk_z * 16);

	for (int y = 0; y < 18; ++y)
	{
		for (int z = 0; z < 18; ++z)
		{
			for (int x = 0; x < 18; ++x)
			{
				glm::ivec3 offset(x - 1, y - 1, z - 1);
				const CMinecraftBlock* block = m_World->GetBlock(ctx.world_position + offset);

				ctx.chunk_data[(y * 18 * 18) + (z * 18) + x] = block;
			}
		}
	}

	GenerateMesh(ctx);
}

void CMinecraftChunkMeshGenerator::OnChunkUnload(std::shared_ptr<ChunkColumn> chunk)
{
	if (chunk == nullptr) 
		return;

	for (int y = 0; y < 16; ++y)
	{
		DestroyChunk(chunk->GetMetadata().x, y, chunk->GetMetadata().z);
	}
}

void CMinecraftChunkMeshGenerator::DestroyChunk(int64 chunk_x, int64 chunk_y, int64 chunk_z)
{
	glm::ivec3 key(chunk_x * 16, chunk_y * 16, chunk_z * 16);

	auto iter = m_ChunkMeshes.find(key);
	if (iter != m_ChunkMeshes.end())
	{
		m_ChunkMeshes.erase(key);
	}
}
