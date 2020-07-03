#include "Framework.h"
#include "InsMesh.h"

UINT InsMesh::instancingIndex = 0;

InsMesh::InsMesh() : RenderingNode()
{
	shader = Shader::Add(L"Mesh");
	material = new Material();
	rasterizerState = new RasterizerState();
	//rasterizerState->FillMode(D3D11_FILL_MODE::D3D11_FILL_WIREFRAME);
}

InsMesh::~InsMesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	delete material;

	delete rasterizerState;
}


bool InsMesh::CreateBuffer()
{
	if (vertexBuffer == NULL || indexBuffer == NULL)
	{
		CreateMesh();

		vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(MeshVertex));
		indexBuffer = new IndexBuffer(indices, indexCount);
	}
	return true;
}

void InsMesh::SetMaterial(wstring diffuseMap, wstring specularMap, wstring normalMap)
{
	material->SetDiffuseMap(diffuseMap);
	material->SetSpecularMap(specularMap);
	material->SetNormalMap(normalMap);
}

void InsMesh::Update()
{
	Super::Update();
}

void InsMesh::Render(Camera* viewer)
{
	Super::Render(viewer);
	
	rasterizerState->SetState();

	material->Render();
}
