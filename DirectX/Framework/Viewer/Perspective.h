#pragma once
#include "Projection.h"

class Perspective : public Projection
{
public:
	Perspective(float width, float height, float zn, float zf, float fov);
	~Perspective();

	void Set(float width, float height, float zn, float zf , float fov) override;

private:
	float aspect;
};