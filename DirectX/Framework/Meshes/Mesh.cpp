#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh() : RenderingNode()
{
	shader = Shader::Add(L"Mesh");
	material = new Material();
	rasterizerState = new RasterizerState();
	//rasterizerState->FillMode(D3D11_FILL_MODE::D3D11_FILL_WIREFRAME);
}

Mesh::~Mesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	delete material;

	delete rasterizerState;
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

void Mesh::SetMaterial(wstring diffuseMap, wstring specularMap, wstring normalMap)
{
	material->SetDiffuseMap(diffuseMap);
	material->SetSpecularMap(specularMap);
	material->SetNormalMap(normalMap);
}

void Mesh::Update()
{
	Super::Update();
}

void Mesh::Render(Camera* viewer)
{
	Super::Render(viewer);
	

	vertexBuffer->Render();
	indexBuffer->Render();

	shader->Render();

	material->Render();

	rasterizerState->SetState();

	D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->DrawIndexed(indexCount, 0,0);
}
