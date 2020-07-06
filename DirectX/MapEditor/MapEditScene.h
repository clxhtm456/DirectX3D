#pragma once

#include "Systems/Scene.h"

class MapEditScene : public Scene
{
public:
	virtual void Initialize() override;
	virtual void Ready() override {};
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {};
private:
	void CreateFreedomCamera();
private:
	class Camera* freedomCam;
	class MapEditor* mapEditor;
	
};
