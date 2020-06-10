#include "Framework.h"
#include "Sky.h"
#include "Scattering.h"
#include "Dome.h"
#include "Moon.h"
#include "Cloud.h"

Sky::Sky(Shader* shader):
	shader(shader)
{
	scatterDesc.InvWaveLength.x = 1.0f / powf(scatterDesc.WaveLength.x, 4.0f);
	scatterDesc.InvWaveLength.y = 1.0f / powf(scatterDesc.WaveLength.y, 4.0f);
	scatterDesc.InvWaveLength.z = 1.0f / powf(scatterDesc.WaveLength.z, 4.0f);

	scatterDesc.WaveLengthMie.x = powf(scatterDesc.WaveLength.x, -0.84f);
	scatterDesc.WaveLengthMie.y = powf(scatterDesc.WaveLength.y, -0.84f);
	scatterDesc.WaveLengthMie.z = powf(scatterDesc.WaveLength.z, -0.84f);

	scattering = new Scattering(shader);
	scatterBuffer = new ConstantBuffer(&scatterDesc, sizeof(ScatterDesc));
	sScatterBuffer = shader->AsConstantBuffer("CB_Scattering");

	cloudBuffer = new ConstantBuffer(&cloudDesc, sizeof(CloudDesc));
	sCloudBuffer = shader->AsConstantBuffer("CB_Cloud");

	dome = new Dome(shader, Vector3(0, 16, 0), Vector3(80, 80, 80));
	moon = new Moon(shader);
	cloud = new Cloud(shader);

	sRayleighMap = shader->AsSRV("RayleighMap");
	sMieMap = shader->AsSRV("MieMap");
}

Sky::~Sky()
{
	delete scatterBuffer;
	delete cloudBuffer;

	delete scattering;
	delete dome;
	delete moon;
	delete cloud;
}

void Sky::Update()
{
	//Auto
	if(bRealTime == true)
	{
		theta += Time::Delta() * timeFactor;

		if (theta > Math::PI)
			theta -= Math::PI * 2.0f;

		float x = sinf(theta);
		float y = cosf(theta);

		Context::Get()->Direction() = Vector3(x, y, 0.0f);
	}

	//Manual
	else
	{
		ImGui::SliderFloat("Theta", &theta,-Math::PI,Math::PI);

		float x = sinf(theta);
		float y = cosf(theta);

		Context::Get()->Direction() = Vector3(x, y, 0.0f);
	}

	static Vector3 domePosition(0, 0, 0);
	static Vector3 domeScale(100, 100, 100);
	ImGui::SliderFloat3("P", domePosition, -100, 100);
	ImGui::SliderFloat3("S", domeScale, 1, 200);
	dome->GetTransform()->Position(domePosition);
	dome->GetTransform()->Scale(domeScale);

	scattering->Update();
	dome->Update();
	moon->Update();
	cloud->Update();
}

void Sky::Render()
{
	/*Vector3 position;
	Context::Get()->GetCamera()->Position(&position);*/

	//Dome
	{
		//position.y -= 0.2f;

		sRayleighMap->SetResource(scattering->RayleighRTV()->SRV());
		sMieMap->SetResource(scattering->MieRTV()->SRV());

		dome->Render();
	}

	//Moon
	{
		moon->Render(theta);
	}

	//Cloud
	{
		cloudBuffer->Apply();
		sCloudBuffer->SetConstantBuffer(cloudBuffer->Buffer());

		cloud->Render();
	}
}

void Sky::PreRender()
{
	scatterBuffer->Apply();
	sScatterBuffer->SetConstantBuffer(scatterBuffer->Buffer());

	scattering->PreRender();
	cloud->PreRender();
}

void Sky::PostRender()
{
	//scattering->PostRender();
	cloud->PostRender();
}

void Sky::RealTime(bool val, float theta, float timeFactor)
{
	bRealTime = val;

	this->theta = theta;
	this->timeFactor = timeFactor;
}

void Sky::ScatteringPass(UINT val)
{
	scattering->Pass(val);
}

void Sky::Pass( UINT domePass, UINT moonPass, UINT cloudPass)
{
	dome->Pass(domePass);
	moon->Pass(moonPass);
	cloud->Pass(cloudPass);
}
