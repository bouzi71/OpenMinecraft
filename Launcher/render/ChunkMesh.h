#ifndef TERRACOTTA_RENDER_CHUNKMESH_H_
#define TERRACOTTA_RENDER_CHUNKMESH_H_

namespace terra
{
namespace render
{

class ChunkMesh
{
public:
	ChunkMesh(IRenderDevice& RenderDevice, std::shared_ptr<IBuffer> buffer, size_t vertex_count);
	ChunkMesh(const ChunkMesh& other);

	ChunkMesh& operator=(const ChunkMesh& other);

	const IGeometry& GetGeometry() const;

	void Render(unsigned int model_uniform);
	void Destroy();

private:
	size_t m_VertexCount;

	IRenderDevice& m_RenderDevice;
	std::shared_ptr<IGeometry> m_Geometry;
	std::shared_ptr<IBuffer> m_Buffer;
};

} // ns render
} // ns terra

#endif
