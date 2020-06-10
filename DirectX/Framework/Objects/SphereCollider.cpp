#include "Framework.h"
#include "SphereCollider.h"

SphereCollider::SphereCollider(Transform* transform, Transform* init)
	: transform(transform), init(init)
{
	float radius = 10.0f;
	for (int y = 0; y <= 10; y++)
	{
		float angleY = (Math::PI/2.0f) - (y*Math::PI)/10.0f;
		for (int x = 0; x < 10; x++)
		{
			float angleX = (x * 2*Math::PI) / 10.0f;
			Vector3 dot = Vector3(radius * sinf(angleX) * cosf(angleY), radius * sinf(angleY), radius * cosf(angleX) * cosf(angleY));
			lines.push_back(dot);
		}
	}
}

SphereCollider::~SphereCollider()
{
}

bool SphereCollider::IsIntersect(SphereCollider* other)
{
	return Collision(this->obb, other->obb);
}

void SphereCollider::Update()
{
	SetObb();
}

void SphereCollider::Render(Color color)
{
	Vector3 dest[110];

	Transform temp;
	temp.World(transform->World());

	if (init != NULL)
		temp.World(init->World() * transform->World());

	Matrix world = temp.World();

	for (UINT i = 0; i < 110; i++)
		D3DXVec3TransformCoord(&dest[i], &lines[i], &world);

	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			int destX = (x + 1) % (10 );
			DebugLine::Get()->RenderLine(dest[10*y+x], dest[10*y+destX], color);
			DebugLine::Get()->RenderLine(dest[10 * y + x], dest[10 * (y + 1) + x], color);
		}
		
	}
}

void SphereCollider::SetObb()
{
	Transform temp;
	temp.World(transform->World());

	if (init != NULL)
		temp.World(init->World() * transform->World());

	temp.Position(&obb.Position);

	D3DXVec3Normalize(&obb.AxisX, &temp.Right());
	D3DXVec3Normalize(&obb.AxisY, &temp.Up());
	D3DXVec3Normalize(&obb.AxisZ, &temp.Foward());

	Vector3 scale;
	temp.Scale(&scale);
	obb.HalfSize = scale * 0.5f;
}

bool SphereCollider::SeperatingPlane(Vector3& position, Vector3& direction, Obb& box1, Obb& box2)
{
	float val = fabsf(D3DXVec3Dot(&position, &direction));

	float val2 = 0.0f;
	val2 += fabsf(D3DXVec3Dot(&(box1.AxisX * box1.HalfSize.x), &direction));
	val2 += fabsf(D3DXVec3Dot(&(box1.AxisY * box1.HalfSize.y), &direction));
	val2 += fabsf(D3DXVec3Dot(&(box1.AxisZ * box1.HalfSize.z), &direction));
	val2 += fabsf(D3DXVec3Dot(&(box2.AxisX * box2.HalfSize.x), &direction));
	val2 += fabsf(D3DXVec3Dot(&(box2.AxisY * box2.HalfSize.y), &direction));
	val2 += fabsf(D3DXVec3Dot(&(box2.AxisZ * box2.HalfSize.z), &direction));

	return  val > val2;
}

bool SphereCollider::Collision(Obb& box1, Obb& box2)
{
	Vector3 position = box2.Position - box1.Position;

	//Box1
	if (SeperatingPlane(position, box1.AxisX, box1, box2) == true) return false;
	if (SeperatingPlane(position, box1.AxisY, box1, box2) == true) return false;
	if (SeperatingPlane(position, box1.AxisZ, box1, box2) == true) return false;

	//Box2
	if (SeperatingPlane(position, box2.AxisX, box1, box2) == true) return false;
	if (SeperatingPlane(position, box2.AxisY, box1, box2) == true) return false;
	if (SeperatingPlane(position, box2.AxisZ, box1, box2) == true) return false;

	//X√‡
	if (SeperatingPlane(position, Cross(box1.AxisX, box2.AxisX), box1, box2) == true) return false;
	if (SeperatingPlane(position, Cross(box1.AxisX, box2.AxisY), box1, box2) == true) return false;
	if (SeperatingPlane(position, Cross(box1.AxisX, box2.AxisZ), box1, box2) == true) return false;

	//X√‡
	if (SeperatingPlane(position, Cross(box1.AxisY, box2.AxisX), box1, box2) == true) return false;
	if (SeperatingPlane(position, Cross(box1.AxisY, box2.AxisY), box1, box2) == true) return false;
	if (SeperatingPlane(position, Cross(box1.AxisY, box2.AxisZ), box1, box2) == true) return false;

	//Z√‡
	if (SeperatingPlane(position, Cross(box1.AxisZ, box2.AxisX), box1, box2) == true) return false;
	if (SeperatingPlane(position, Cross(box1.AxisZ, box2.AxisY), box1, box2) == true) return false;
	if (SeperatingPlane(position, Cross(box1.AxisZ, box2.AxisZ), box1, box2) == true) return false;

	return true;
}

Vector3 SphereCollider::Cross(Vector3& vec1, Vector3& vec2)
{
	float x = vec1.y * vec2.z - vec1.z * vec2.y;
	float y = vec1.z * vec2.x - vec1.x * vec2.z;
	float z = vec1.x * vec2.y - vec1.y * vec2.x;

	return Vector3(x, y, z);
}
