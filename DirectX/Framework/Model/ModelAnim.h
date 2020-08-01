#pragma once

struct FrameData
{
	float time;

	Vector3 scale;
	Quaternion rotation;
	Vector3 position;
};


class ModelKeyFrame
{
private:
	friend class ModelClip;

	string boneName;

	float duration;
	float frameRate;
	int frameCount;

	vector<FrameData> transforms;

public:
	ModelKeyFrame();
	~ModelKeyFrame();

	Matrix GetInterpolatedMatrix(float time);
	Matrix GetInterpolatedMatrix(UINT index1, UINT index2, float t);

	UINT GetKeyFrameIndex(float time);
	void CalcKeyFrameIndex(float time, OUT UINT& index1, OUT UINT& index2,
		OUT float& interpolatedTime);
};

class ModelClip
{
private:
	string name;

	float duration;
	float frameRate;
	int frameCount;

	bool isLockRoot;
	bool isRepeat;
	float speed;
	float playTime;

	unordered_map<string, ModelKeyFrame*> keyFrameMap;

	function<void()> EndEvent;
public:
	ModelClip(string file);
	~ModelClip();

	Matrix GetKeyFrameMatrix(ModelBone* bone);
	void UpdateKeyFrame(ModelBone* bone);

	void ReadAnimation(string file);

	void Reset();

	void LockRoot(bool val) { isLockRoot = val; }
	void Repeat(bool val) { isRepeat = val; }
	void Speed(float val) { speed = val; }
	void StartTime(float val) { playTime = val; }

	void SetEndEvent(function<void()> Event) { EndEvent = Event; }
};

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