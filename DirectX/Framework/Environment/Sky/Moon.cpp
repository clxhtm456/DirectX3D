#include "Framework.h"
#include "Moon.h"

Moon::Moon(Shader * shader) : 
	Renderer(shader),
	distance(75),
	glowDistance(70)
{
	moon = new Texture(L"Environment/Moon.png");
	moonGlow = new Texture(L"Environment/Moonglow.png");

	sMoon = shader->AsSRV("MoonMap");
	sAlpha = shader->AsScalar("MoonAlpha");

	VertexTexture vertices[6];
	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 1);
	vertices[4].Uv = Vector2(0, 0);
	vertices[5].Uv = Vector2(1, 0);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(VertexTexture));
}

Moon::~Moon()
{
	SafeDelete(moon);
	SafeDelete(moonGlow);
}

void Moon::Update()
{
	Super::Update();
}

void Moon::Render(float theta)
{

	sAlpha->SetFloat(GetAlpha(theta));

	//Moon
	{
		Matrix W = GetTransform(theta);
		transform->World(W);

		Super::Render();

		sMoon->SetResource(moon->SRV());
		shader->Draw(0, Pass(), 6);
	}

	//Glow
	{
		Matrix W = GetGlowTransform(theta);
		transform->World(W);

		Super::Render();

		sMoon->SetResource(moonGlow->SRV());
		shader->Draw(0, Pass(), 6);
	}
}

float Moon::GetAlpha(float theta)
{
	if (theta < Math::PI * 0.5f || theta > Math::PI * 1.5f)
		return fabsf(sinf(theta + Math::PI / 2.0f));

	return 0;
}

Matrix Moon::GetTransform(float theta)
{
	Vector3 position(0, 0, 0);
	Context::Get()->GetCamera()->Position(&position);

	Matrix S, R, T, D;
	D3DXMatrixScaling(&S, 5, 5, 1);
	D3DXMatrixRotationYawPitchRoll(&R, Math::PI*0.5f, theta - (Math::PI*0.5f), 0);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	Vector3 direction = Context::Get()->Direction() * distance;

	D3DXMatrixTranslation(&D, direction.x, direction.y, direction.z);
	return S * R*T * D;
}

Matrix Moon::GetGlowTransform(float theta)
{
	Vector3 position(0,0,0);
	Context::Get()->GetCamera()->Position(&position);

	Matrix S, R, T, D;
	D3DXMatrixScaling(&S, 10, 10, 1);
	D3DXMatrixRotationYawPitchRoll(&R, Math::PI*0.5f, theta - (Math::PI*0.5f), 0);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	Vector3 direction = Context::Get()->Direction() * glowDistance;

	D3DXMatrixTranslation(&D, direction.x, direction.y, direction.z);
	return S * R*T * D;
}
