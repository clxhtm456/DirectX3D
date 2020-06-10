#pragma once

class SphereCollider
{
private:
	struct Obb;

public:
	SphereCollider(Transform* transform, Transform* init = NULL);
	~SphereCollider();

	bool IsIntersect(SphereCollider* other);

	void Update();
	void Render(Color color);

	Transform* GetTransform() { return transform; }

private:
	void SetObb();

private:
	bool SeperatingPlane(Vector3& position, Vector3& direction, Obb& box1, Obb& box2);
	bool Collision(Obb& box1, Obb& box2);
	Vector3 Cross(Vector3& vec1, Vector3& vec2);


private:
	struct Obb
	{
		Vector3 Position;

		Vector3 AxisX;
		Vector3 AxisY;
		Vector3 AxisZ;

		Vector3 HalfSize;
	} obb;

private:
	Transform* init;
	Transform* transform;

	vector<Vector3> lines;
};