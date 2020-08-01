#pragma once

class ModelTweener
{
private:
	class ModelClip* current;
	class ModelClip* next;

	float blendTime;
	float elapsedTime;

	//bool isPlaying = false;

public:
	ModelTweener();
	~ModelTweener();

	void Play(class ModelClip* clip, bool isRepeat = false, float blendTime = 10.0f,
		float speed = 15.0f, float startTime = 0.0f);

	//bool IsPlaying() { return isPlaying; }

	void UpdateBlending(class ModelBone* bone);
};