#include "Framework.h"
#include "DebugLine.h"

DebugLine* DebugLine::instance = NULL;

void DebugLine::Create()
{
	assert(instance == NULL);

	instance = new DebugLine();
}

void DebugLine::Delete()
{
	SafeDelete(instance);
}

DebugLine * DebugLine::Get()
{
	assert(instance != NULL);

	return instance;
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end)
{
	RenderLine(start, end, Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end, float r, float g, float b)
{
	RenderLine(start, end, Color(r, g, b, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2, Color & color)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), color);
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2, float r, float g, float b)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), Color(r, g, b, 1));
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end, Color & color)
{
	vertices[drawCount].Color = color;
	vertices[drawCount++].Position = start;

	vertices[drawCount].Color = color;
	vertices[drawCount++].Position = end;
}

void DebugLine::Render()
{
	Context::Get()->GetMainCamera()->GetVPBuffer()->SetVSBuffer(0);
	shader->Binding();

	/*D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, vertices, sizeof(VertexColor) * MAX_LINE_VERTEX);
	}
	D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);*/

	vertexBuffer->Binding();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	D3D::GetDC()->Draw(drawCount * 2,0);

	drawCount = 0;
	ZeroMemory(vertices, sizeof(VertexColor) * MAX_LINE_VERTEX);
}

DebugLine::DebugLine()
{
	shader = Shader::Add(L"DebugLine");

	vertices = new VertexColor[MAX_LINE_VERTEX];
	ZeroMemory(vertices, sizeof(VertexColor) * MAX_LINE_VERTEX);

	vertexBuffer = new VertexBuffer(vertices, MAX_LINE_VERTEX, sizeof(VertexColor), 0, true);
}

DebugLine::~DebugLine()
{
	SafeDeleteArray(vertices);
	SafeDelete(vertexBuffer);
}
