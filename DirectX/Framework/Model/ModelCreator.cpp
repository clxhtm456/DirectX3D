#include "Framework.h"
#include "ModelCreator.h"

vector<wstring> ModelCreator::modelDirList;
vector<ModelInstance*> ModelCreator::modelAnimList;

ModelCreator* ModelCreator::Create(Shader* shader,wstring dir)
{
	ModelCreator* temp = new ModelCreator();
	if (!temp || !temp->Init(shader,dir))
	{
		delete temp;
		temp = nullptr;
	}
	return temp;
}

void ModelCreator::Update()
{
	for (auto instance : modelAnimList)
	{
		instance->modelAnimator->Update();
		instance->modelAnimator->UpdateTransforms();
	}
}

void ModelCreator::Render()
{
	for (auto instance : modelAnimList)
	{
		instance->modelAnimator->Render();
	}
}

bool ModelCreator::Init(Shader* shader,wstring dir)
{
	int exist = -1;
	for (int i = 0; i < modelDirList.size(); i++)
	{
		if (modelDirList[i] == dir)
			exist = i;
	}

	if (exist == -1)//존재하지 않는 instance 새로 생성 필요
	{
		ModelInstance* instance = new ModelInstance();
		animator = new ModelAnimator(shader);
		instance->index = 0;
		instance->modelAnimator = animator;

		index = 0;
#if _USE_CACHE_DIR
		animator->ReadModel(dir);
#else
		animator->ReadMaterial(dir);
		animator->ReadMesh(dir);
#endif
		modelAnimList.push_back(instance);
		modelDirList.push_back(dir);

		transform = animator->AddTransform();
		model = animator->GetModel();
	}
	else//이미 생성된 instance
	{
		ModelInstance* instance = modelAnimList[exist];
		animator = instance->modelAnimator;
		index = ++instance->index;

		transform = animator->AddTransform();
		model = animator->GetModel();
	}

	if (animator == nullptr)
		return false;
	else
		return true;
}

void ModelCreator::AddAnimation(wstring dir)
{
#if _USE_CACHE_DIR
	animator->ReadAnimation(dir);
#else
	animator->ReadClip(dir);
#endif
}

void ModelCreator::PlayClip(UINT clip,float speed, float takeTime)
{
	animator->PlayClip(index, clip, speed, takeTime);

	animator->UpdateTransforms();
}

void ModelCreator::Pass(UINT index)
{
	animator->Pass(index);
}
