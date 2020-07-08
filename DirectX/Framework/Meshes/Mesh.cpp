#include "Framework.h"
#include "Mesh.h"

#include "Objects/EmptyNode.h"



Mesh::Mesh() : RenderingNode()
{
	shader = Shader::Add(L"InstMesh");
	material = new Material();
	rasterizerState = new RasterizerState();

	instancingCount = 0;

	bInstancingMode = false;

	for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
		worlds[i] = XMMatrixIdentity();

	instancingBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);

	instancingCount = 1;
}

Mesh::~Mesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	delete material;

	delete rasterizerState;

	delete instancingBuffer;
	vector<Node*> releaseList;
	for (auto iter = instanceMatrixList.begin(); iter != instanceMatrixList.end(); iter++)
	{
		releaseList.push_back((*iter).first);
	}
	//메모리 꼬임방지용
	for (auto object : releaseList)
	{
		object->Release();
	}
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
	VPSet(viewer);
	//WorldSet();
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

		instancingBuffer->Render();
		D3D::GetDC()->DrawIndexedInstanced(indexCount, instancingCount, 0, 0, 0);
	}
}

void Mesh::CalcWorldMatrix()
{
	if (bInstancingMode == true)
		return;

	worlds[0] = GetWorld();
	worlds[0] = XMMatrixTranspose(worlds[0]);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instancingBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instancingBuffer->Buffer(), 0);
}

Node* Mesh::CreateInstance()
{
	StartInstancingMode();

	Node* object = EmptyNode::Create();

	int a = 0;
	IncreaseInstancing(object);
	object->OnDestroy = std::bind(&Mesh::DecreaseInstancing, this,std::placeholders::_1);
	object->OnChangePosition = [&,object](Matrix matrix)->void
	{
		for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
			worlds[i] = XMMatrixIdentity();

		instanceMatrixList.at(object) = matrix;

		int i = 0;
		for (auto iter = instanceMatrixList.begin(); iter != instanceMatrixList.end(); iter++, i++)
		{
			memcpy(&worlds[i], &(*iter).second, sizeof(Matrix));
			worlds[i] = XMMatrixTranspose(worlds[i]);
		}

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(instancingBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
		}
		D3D::GetDC()->Unmap(instancingBuffer->Buffer(), 0);

	};

	return object;
}

void Mesh::StartInstancingMode()
{
	if (bInstancingMode == false)
	{
		bInstancingMode = true;
		instancingCount = 0;
	}
	else
		return;

}

void Mesh::IncreaseInstancing(Node* object)
{
	instanceMatrixList.insert(std::pair<Node*, Matrix>(object, object->GetWorld()));

	instancingCount++;
}

void Mesh::DecreaseInstancing(Node* object)
{
	assert(instancingCount > 0);

	instanceMatrixList.erase(object);
	instancingCount--;
}

void Mesh::UpdateInstancingMatrix()
{
	
}
