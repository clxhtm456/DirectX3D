#include "Framework.h"
#include "ModelAnim.h"

#include "ModelData.h"
#include "ModelTweener.h"

ModelAnim* ModelAnim::Create(string modelDir)
{
	auto pRet = new ModelAnim();
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

bool ModelAnim::Init(string modelDir)
{
	if (!Super::Init(modelDir))
		return false;

	SetShader(L"InstModel");
	GetModelData()->GetBuffer()->SetBlend(true);
	tweener = new ModelTweener();

	return true;
}

ModelAnim::ModelAnim():
	Model()
{
}

ModelAnim::~ModelAnim()
{
	for (ModelClip* clip : clips)
		delete clip;

	delete tweener;
}

void ModelAnim::Update()
{
	if (clips.empty())
		return;

	vector<ModelBone*>* bones = (modelData->GetBones());

	for (UINT i = 0; i < bones->size(); i++)
	{
		ModelBone* bone = bones->at(i);
		tweener->UpdateBlending(bone);
	}

	CalcWorldMatrix();
}

UINT ModelAnim::AddClip(string file)
{
	file = "../../_Assets/Clips/" + file + ".clip";
	ModelClip* clip = new ModelClip(file);
	clips.push_back(clip);

	if(clips.size() == 1)
		PlayClip(0, true);

	return clips.size() - 1;
}

void ModelAnim::PlayClip(UINT index, bool isRepeat, float blendTime, float speed, float startTime)
{
	if ((clips.size() - 1) < index)
		return;

	tweener->Play(clips[index], isRepeat, blendTime, speed, startTime);
}
