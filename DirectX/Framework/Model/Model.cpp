#include "Framework.h"
#include "Model.h"

#include "ModelData.h"
#include "Objects/EmptyNode.h"

Model* Model::Create(string modelDir)
{
	auto pRet = new Model();
	if (pRet && pRet->Init(modelDir))
	{
		pRet->AutoRelease();
	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}
	return pRet;
}

bool Model::Init(string modelDir)
{
	modelData = new ModelData(modelDir);
	shader = Shader::Add(L"InstModel");
	intBuffer = new IntBuffer();

	//rasterizerState->FillMode(D3D11_FILL_WIREFRAME);

	bInstancingMode = false;

	for (UINT i = 0; i < MAX_MESH_INSTANCE; i++)
		worlds[i] = XMMatrixIdentity();

	instancingBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);

	instancingCount = 1;

	return true;
}

Model::Model()
{
}

Model::~Model()
{
	delete modelData;
	delete intBuffer;

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

Node* Model::CreateInstance()
{
	StartInstancingMode();

	Node* object = EmptyNode::Create();

	IncreaseInstancing(object);

	return object;
}

void Model::StartInstancingMode()
{
	if (bInstancingMode == false)
	{
		bInstancingMode = true;
		instancingCount = 0;
	}
	else
		return;
}

void Model::IncreaseInstancing(Node * object)
{
	instanceMatrixList.insert(std::pair<Node*, Matrix>(object, object->GetWorld()));

	instancingCount++;

	object->OnDestroy = std::bind(&Model::DecreaseInstancing, this, std::placeholders::_1);
	object->OnChangePosition = [&, object](Matrix matrix)->void
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
}

void Model::DecreaseInstancing(Node * object)
{
	assert(instancingCount > 0);

	instanceMatrixList.erase(object);
	instancingCount--;
}

void Model::UpdateInstancingMatrix()
{
}

void Model::CalcWorldMatrix()
{
	Super::CalcWorldMatrix();
	modelData->CopyGlobalBoneTo(boneTransforms, GetWorld());
}

void Model::Update()
{
}

void Model::Render(Camera* viewer)
{
	VPSet(viewer);
	LightSet();

	modelData->SetBones(boneTransforms.data(), boneTransforms.size());

	vector<ModelMesh*>* meshes = modelData->GetMeshes();

	for (ModelMesh* mesh : *meshes)
	{
		int index = mesh->GetParentBoneIndex();

		intBuffer->data.index = index;
		intBuffer->SetVSBuffer(3);

		shader->Render();
		//rstate->SetState();
		instancingBuffer->Render();
		mesh->SetInstancingCount(instancingCount);
		mesh->Render();
	}
}
