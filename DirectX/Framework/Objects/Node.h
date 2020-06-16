#pragma once

class Node
{
protected:
	Node();
	virtual ~Node();
public:
	virtual bool Init();
	virtual void PostUpdate();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Render();
	virtual void RemoveFromParent();
protected:
	virtual void Draw();
public:
	Vector3 GetPosition();
	Vector3 GetRotation();
	Vector3 GetRotationDegree();
	Vector3 GetScale();

	void SetPosition(Vector3 position);
	void SetRotation(Vector3 rotation);
	void SetRotationDegree(Vector3 rotation);
	void SetScale(Vector3 scale);

	bool GetRunning() { return _running; }
public:
	void AddChild(Node* child);
	void DelChild(Node* child);
public:
	void AutoRelease();
	void Release();
	void Retain();
private:
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