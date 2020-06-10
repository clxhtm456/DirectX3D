#pragma once

struct ModelInstance
{
public:
	ModelAnimator* modelAnimator;
	UINT index;
};

class ModelCreator
{
public:
	static ModelCreator* Create(Shader* shader, wstring dir);

	static void Update();
	static void Render();
protected:
	bool Init(Shader* shader, wstring dir);
public :
	void AddAnimation(wstring dir);
	void PlayClip(UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void Pass(UINT index);

	Matrix GetAttachTransform() { return animator->GetAttachTransform(index); }
	Transform* GetTransform() { return transform; }
	Model* GetModel() { return model; }
private:
	UINT index;
	Transform* transform;
	Model* model;
	ModelAnimator* animator;
private:
	static vector<wstring> modelDirList;
	static vector<ModelInstance*> modelAnimList;
};