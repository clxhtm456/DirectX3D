#pragma once

class Node
{
private:
	friend class Scene;
protected:
	Node();
	virtual ~Node();
protected:
	virtual void Start() = 0;
	virtual void PostUpdate() = 0;
	virtual void Update() = 0;
	virtual void LateUpdate() = 0;
	virtual void Render(Camera* viewer) = 0;
	virtual void PreRender(Camera* viewer) = 0;
	virtual void PostRender(Camera* viewer) = 0;
	virtual void RemoveFromParent() = 0;
protected:
	void AutoPostUpdate();
	void AutoUpdate();
	void AutoLateUpdate();
	void AutoRender(Camera* viewer = nullptr);
	void AutoPreRender(Camera* viewer = nullptr);
	void AutoPostRender(Camera* viewer = nullptr);
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

	void AddEvent(const std::function<void()>& func, float timer = 0.0f);
	virtual UINT ChildType()
	{
		return TYPE_NODE;
	}
public:
	void AutoRelease();
	void Release();
	void Retain();
protected:
	XMVECTOR _position;
	XMVECTOR _rotation;
	XMVECTOR _scale;
protected:
	Scene* _scene;

private:
	unsigned int _referenceCount;
	Node* _parent;
	list<Node*> _childList;
	vector<pair<std::function<void()>, float>> _eventList;
	bool _reorderChildDirty;

	bool _running;
	bool _visible;
};