#include "stdafx.h"

#include "ChunkMesh.h"

#include "../assets/AssetCache.h"

CMinecraftChunkMesh::CMinecraftChunkMesh(IRenderDevice& RenderDevice, std::shared_ptr<IBuffer> buffer)
	: m_RenderDevice(RenderDevice)
	, m_Buffer(buffer)
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
}

CMinecraftChunkMesh& CMinecraftChunkMesh::operator=(const CMinecraftChunkMesh& other)
{
	this->m_Geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
	this->m_Geometry->SetVertexBuffer(other.m_Buffer);

	this->m_Buffer = other.m_Buffer;
	return *this;
}

const IGeometry& CMinecraftChunkMesh::GetGeometry() const
{
	return *m_Geometry;
}
