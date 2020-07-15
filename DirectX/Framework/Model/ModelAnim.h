#pragma once

class ModelAnim : public Model
{
public:
	static ModelAnim* Create(string modelDir);
	bool Init(string modelDir);
protected:
	ModelAnim();
	virtual ~ModelAnim();

	void Update() override;
public:
	UINT AddClip(string file);
	void PlayClip(UINT index, bool isRepeat = false, float blendTime = 10.0f,
		float speed = 15.0f, float startTime = 0.0f);

protected:
	vector<class ModelClip*> clips;
	class ModelTweener* tweener;


};