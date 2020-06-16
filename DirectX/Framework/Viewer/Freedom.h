#pragma once
#include "Camera.h"

class Freedom : public Camera
{
public:
	Freedom(CameraOption option);
	~Freedom();

	void Update() override;
	void Speed(float move, float rotation);


private:
	float move = 20.0f;
	float rotation = 5.0f;

};
