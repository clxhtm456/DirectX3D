#pragma once
#include "Camera.h"

class Freedom : public Camera
{
public:
	static Freedom* Create(CameraOption option);
public:
	Freedom();
	~Freedom();

	void Update() override;
	void Speed(float move, float rotation);


private:
	float move = 10.0f;
	float rotation = 2.0f;

};
