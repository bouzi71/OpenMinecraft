#ifndef TERRACOTTA_RENDER_CHUNKMESH_H_
#define TERRACOTTA_RENDER_CHUNKMESH_H_

class CMinecraftChunkMesh
{
public:
	CMinecraftChunkMesh(IRenderDevice& RenderDevice, std::shared_ptr<IBuffer> buffer);
	CMinecraftChunkMesh(const CMinecraftChunkMesh& other);

	CMinecraftChunkMesh& operator=(const CMinecraftChunkMesh& other);

	const IGeometry& GetGeometry() const;

private:
	IRenderDevice& m_RenderDevice;
	std::shared_ptr<IGeometry> m_Geometry;
	std::shared_ptr<IBuffer> m_Buffer;
};

#endif
