#pragma once

class Node : public AlignedAllocationPolicy<16>
{
private:
	friend class Scene;
protected:
	Node();
	virtual ~Node();

	bool Init();
protected:
	/*
	*Start 
	*Update ���� ���� ó�� �ѹ� ����
	*/
	virtual void Start();
	/*
	*PostUpdate
	*Update �������� ���� ����
	*/
	virtual void PostUpdate();
	/*
	*Update
	*�� �����Ӹ��� ����
	*/
	virtual void Update() = 0;
	/*
	*LateUpdate
	*��� Update�� ������� ����
	*/
	virtual void LateUpdate();
	/*
	*ResourceBinding
	*���̴� ���ҽ� �Ҵ����
	*/
	virtual void ResourceBinding(class Camera* viewer);
	/*
	*Render
	*���̴� ������ ����
	*/
	virtual void Render(class Camera* viewr = nullptr);
	/*
	*PostRender
	*��� �������� ��������
	*/
	virtual void PostRender(class Camera* viewer);
	void Destroy();
private:
	void AutoPostUpdate();
	void AutoUpdate();
	void AutoLateUpdate();
	void AutoRender(class Camera* viewer = nullptr);
	void AutoPostRender(class Camera* viewer = nullptr);

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

	void RemoveFromParent();
	bool GetRunning() { return _running; }
	Scene* GetScene() { return _scene; }
public:
	void AddChild(Node* child);
	void DelChild(Node* child);

	void AddEvent(const std::function<void()>& func, float timer = 0.0f);
	virtual UINT ChildType()
	{
		return TYPE_NODE;
	}
public:
	UINT GetObjectMask() { return objectMask; }
	void AddMask(UINT mask);
	void DelMask(UINT mask);
	void SetDefaultMask();
private:
	UINT objectMask = 0;
public:
	void AutoRelease();
	void Release();
	void Retain();
protected:
	XMVECTOR _position = XMVectorSet(0,0,0,0);
	XMVECTOR _rotation = XMVectorSet(0, 0, 0, 0);
	XMVECTOR _scale = XMVectorSet(1, 1, 1, 0);
protected:
	Scene* _scene;
public://callbackList
	/*
	*OnDestroy
	*������Ʈ �������� ȣ���
	*/
	std::function<void(Node* self)> OnDestroy = NULL;
	/*
	*OnStart
	*Start�Լ� ȣ��� ȣ���
	*/
	std::function<void(Node* self)> OnStart = NULL;
	/*
	*OnChangePosition
	*Transform�� ����ɽ� ȣ���
	*/
	std::function<void(Matrix)> OnChangePosition = NULL;
public:
	Matrix GetWorld() { return _world; }
private:
	unsigned int _referenceCount;
	Node* _parent;
	list<Node*> _childList;
	vector<pair<std::function<void()>, float>> _eventList;
	bool _reorderChildDirty;

	bool _running;
	bool _visible;
protected:
	Matrix _world;
};