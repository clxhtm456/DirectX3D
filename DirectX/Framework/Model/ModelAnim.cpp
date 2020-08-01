#include "Framework.h"
#include "ModelAnim.h"
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

ModelKeyFrame::ModelKeyFrame()
{
}

ModelKeyFrame::~ModelKeyFrame()
{
}

Matrix ModelKeyFrame::GetInterpolatedMatrix(float time)
{
	return Matrix();
}

Matrix ModelKeyFrame::GetInterpolatedMatrix(UINT index1, UINT index2, float t)
{
	return Matrix();
}

UINT ModelKeyFrame::GetKeyFrameIndex(float time)
{
	return 0;
}

void ModelKeyFrame::CalcKeyFrameIndex(float time, OUT UINT& index1, OUT UINT& index2, OUT float& interpolatedTime)
{
}

ModelClip::ModelClip(string file)
{
}

ModelClip::~ModelClip()
{
}

Matrix ModelClip::GetKeyFrameMatrix(ModelBone* bone)
{
	return Matrix();
}

void ModelClip::UpdateKeyFrame(ModelBone* bone)
{
}

void ModelClip::ReadAnimation(string file)
{
}

void ModelClip::Reset()
{
}
