#include "Framework.h"
#include "Model.h"

#include "ModelData.h"

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
	shader = Shader::Add(L"Model");
	intBuffer = new IntBuffer();

	rasterizerState->FillMode(D3D11_FILL_WIREFRAME);

	return true;
}

Model::Model()
{
}

Model::~Model()
{
	delete modelData;
	delete intBuffer;
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
	//Super::Render(viewer);

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
		mesh->Render();
	}
}
