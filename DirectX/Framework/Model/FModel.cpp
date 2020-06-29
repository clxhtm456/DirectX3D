#include "Framework.h"
#include "FModel.h"

#include "FModelData.h"

FModel* FModel::Create(string modelDir)
{
	auto pRet = new FModel();
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

bool FModel::Init(string modelDir)
{
	//modelData = new FModelData(modelDir);
	shader = Shader::Add(L"Model");
	intBuffer = new IntBuffer();
	rstate = new RasterizerState();

	rstate->FillMode(D3D11_FILL_WIREFRAME);

	return true;
}

FModel::FModel()
{
}

FModel::~FModel()
{
	delete modelData;
	delete intBuffer;

	delete rstate;
}

void FModel::CalcWorldMatrix()
{
	//modelData->CopyGlobalBoneTo(boneTransforms, GetWorld());

	Super::CalcWorldMatrix();
}

void FModel::Update()
{
}

void FModel::Render(Camera* viewer)
{
	////Super::Render(viewer);

	//VPSet(viewer);
	//LightSet();

	//modelData->SetBones(boneTransforms.data(), boneTransforms.size());

	//vector<ModelMesh*> meshes = modelData->GetMeshes();

	//for (ModelMesh* mesh : meshes)
	//{
	//	int index = mesh->GetParentBoneIndex();

	//	intBuffer->data.index = index;
	//	intBuffer->SetVSBuffer(3);

	//	shader->Render();
	//	rstate->SetState();
	//	mesh->Render();
	//}
}
