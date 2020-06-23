#pragma once

class Node
{
protected:
	Node();
	virtual ~Node();
protected:
	virtual bool Init() = 0;
	virtual void PostUpdate() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Render() = 0;
	virtual void PreRender() = 0;
	virtual void PostRender() = 0;
	virtual void RemoveFromParent() = 0;
public:
	bool AutoInit();
	void AutoPostUpdate();
	void AutoUpdate();
	void AutoLateUpdate();
	void AutoRender();
	void AutoPreRender();
	void AutoPostRender();
protected:
	virtual void Draw();
public:
	Vector3 GetPosition();
	Vector3 GetRotation();
	Vector3 GetRotationDegree();
	Vector3 GetScale();

	virtual void SetPosition(Vector3 position);
	virtual void SetRotation(Vector3 rotation);
	virtual void SetRotationDegree(Vector3 rotation);
	virtual void SetScale(Vector3 scale);

	void SetPosition(float x, float y , float z);
	void SetRotation(float x, float y, float z);
	void SetRotationDegree(float x, float y, float z);
	void SetScale(float x, float y, float z);

	bool GetRunning() { return _running; }
public:
	void AddChild(Node* child);
	void DelChild(Node* child);
public:
	void AutoRelease();
	void Release();
	void Retain();
protected:
	XMVECTOR _position;
	XMVECTOR _rotation;
	XMVECTOR _scale;

private:
	unsigned int _referenceCount;

	Node* _parent;
	vector<Node*> _childList;
	vector<pair<std::function<void()>, float>> _eventList;
	bool _reorderChildDirty;

	bool _running;
	bool _visible;
};