#include "stdafx.h"

#include "ChunkMesh.h"

#include "../assets/AssetCache.h"

namespace terra
{
namespace render
{

ChunkMesh::ChunkMesh(IRenderDevice& RenderDevice, std::shared_ptr<IBuffer> buffer, size_t vertex_count)
	: m_RenderDevice(RenderDevice)
	, m_Buffer(buffer)
	, m_VertexCount(vertex_count)
{
	m_Geometry = RenderDevice.GetObjectsFactory().CreateGeometry();
	m_Geometry->SetVertexBuffer(m_Buffer);
}

ChunkMesh::ChunkMesh(const ChunkMesh& other)
	: m_RenderDevice(other.m_RenderDevice)
{
	this->m_Geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
	this->m_Geometry->SetVertexBuffer(other.m_Buffer);
	this->m_Buffer = other.m_Buffer;
	this->m_VertexCount = other.m_VertexCount;
}

ChunkMesh& ChunkMesh::operator=(const ChunkMesh& other)
{
	this->m_Geometry = m_RenderDevice.GetObjectsFactory().CreateGeometry();
	this->m_Geometry->SetVertexBuffer(other.m_Buffer);

	this->m_Buffer = other.m_Buffer;
	this->m_VertexCount = other.m_VertexCount;

	return *this;
}

const IGeometry & ChunkMesh::GetGeometry() const
{
	return *m_Geometry;
}

void ChunkMesh::Render(unsigned int model_uniform)
{
	static const glm::mat4 modelMatrix(1.0);

	/*glBindVertexArray(m_VAO);
	g_AssetCache->GetTextures().Bind();

	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	//glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);

	GLenum error;

	while ((error = glGetError()) != GL_NO_ERROR)
	{
		std::cout << "OpenGL error rendering: " << error << std::endl;
	}*/
}

void ChunkMesh::Destroy()
{
	/*glBindVertexArray(m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);*/
}

} // ns render
} // ns terra
