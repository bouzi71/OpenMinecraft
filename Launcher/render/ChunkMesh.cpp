#include "stdafx.h"

#include "ChunkMesh.h"

#include "../assets/AssetCache.h"

CMinecraftChunkMesh::CMinecraftChunkMesh(IRenderDevice& RenderDevice, std::shared_ptr<IBuffer> buffer, size_t vertex_count)
	: m_RenderDevice(RenderDevice)
	, m_Buffer(buffer)
	, m_VertexCount(vertex_count)
{
	m_Geometry = RenderDevice.GetObjectsFactory().CreateGeometry();
	m_Geometry->SetVertexBuffer(m_Buffer);
}

CMinecraftChunkMesh::CMinecraftChunkMesh(const CMinecraftChunkMesh& other)
	: m_RenderDevice(other.m_RenderDevice)
{
	this->m_Geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
	this->m_Geometry->SetVertexBuffer(other.m_Buffer);
	this->m_Buffer = other.m_Buffer;
	this->m_VertexCount = other.m_VertexCount;
}

CMinecraftChunkMesh& CMinecraftChunkMesh::operator=(const CMinecraftChunkMesh& other)
{
	this->m_Geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
	this->m_Geometry->SetVertexBuffer(other.m_Buffer);

	this->m_Buffer = other.m_Buffer;
	this->m_VertexCount = other.m_VertexCount;

	return *this;
}

const IGeometry & CMinecraftChunkMesh::GetGeometry() const
{
	return *m_Geometry;
}
