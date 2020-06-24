#pragma once

class Scene
{
private:
	friend class Main;
public:
	virtual void Initialize() = 0;
	virtual void Ready() = 0;
	virtual void Destroy() = 0;

	virtual void Update() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

	virtual void ResizeScreen() = 0;
private:
	void AutoInitialize();
	void AutoDestroy();
	void AutoUpdate();
	void AutoPreRender();
	void AutoRender();
	void AutoPostRender();
public:
	void AddChild(Node* node);
	void DelChild(Node* child);

	void SetMainCamera(Camera* cam);
private:
	vector<Node*> _childList;
	class Camera* _mainCamera = nullptr;
};