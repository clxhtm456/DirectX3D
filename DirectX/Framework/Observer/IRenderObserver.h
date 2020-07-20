#pragma once

class IRenderObserver
{
public:
	virtual void ReleaseBuffer() = 0;
	virtual void CreateBuffer(float width, float height) = 0;
};