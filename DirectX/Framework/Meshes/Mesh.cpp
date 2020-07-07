#include "Framework.h"
#include "Mesh.h"

#include "InstancingObject.h"



Mesh::Mesh() : RenderingNode()
{
	shader = Shader::Add(L"Mesh");
	material = new Material();
	rasterizerState = new RasterizerState();

	instancingCount = 0;

	for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
		worlds[i] = XMMatrixIdentity();

	instancingBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
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
	
	rasterizerState->SetState();

	material->Render();
}

void Mesh::Draw(Camera * viewer)
{
	if (instancingCount < 1)
		return;

	VPSet(viewer);

	LightSet();

	if (vertexBuffer != NULL && indexBuffer != NULL)
	{
		vertexBuffer->Render();
		indexBuffer->Render();


		if (vsShader == NULL && psShader == NULL)
			shader->Render();
		else
		{
			vsShader->RenderVS();
			psShader->RenderPS();
		}

		D3D::GetDC()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D::GetDC()->DrawIndexedInstanced(indexCount, instancingCount, 0, 0,0);
	}
}

Node* Mesh::CreateInstance()
{
	Node* object = InstancingObject::Create();

	IncreaseInstancing(object);
	object->OnDestroy = std::bind(&Mesh::DecreaseInstancing, this,std::placeholders::_1);
	object->OnChangePosition = [=](Matrix matrix)->void
	{
		for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
			worlds[i] = XMMatrixIdentity();

		for (UINT i = 0; i < instancingCount; i++)
		{
			memcpy(&worlds[i], &instanceNode[i]->GetWorld(), sizeof(Matrix));
		}

		instancingBuffer->Render();
	};

	return object;
}

void Mesh::IncreaseInstancing(Node* object)
{
	instanceNode.push_back(object);

	instancingCount++;
}

void Mesh::DecreaseInstancing(Node* object)
{
	assert(instancingCount > 0);

	for (auto iter = instanceNode.begin(); iter != instanceNode.end(); iter++)
	{
		if (*iter == object)
		{
			instanceNode.erase(iter);
			instancingCount--;
		}
	}
}

void Mesh::UpdateInstancingMatrix()
{
	
}
