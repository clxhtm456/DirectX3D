#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh() : RenderingNode()
{
	shader = Shader::Add(L"Mesh");
	texture = Texture::Add(L"boxTexture.png");
}

Mesh::~Mesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);
}


bool Mesh::CreateBuffer()
{
	if (vertexBuffer == NULL || indexBuffer == NULL)
	{
		CreateMesh();

		vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(MeshVertex));
		indexBuffer = new IndexBuffer(indices, indexCount);
	}
	return true;
}

void Mesh::Update()
{
	Super::Update();
}

void Mesh::Render()
{
	Super::Render();
	

	vertexBuffer->Render();
	indexBuffer->Render();

	shader->Render();

	texture->Set(0);

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->DrawIndexed(indexCount, 0,0);
}
