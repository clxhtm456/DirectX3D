#pragma once

class Projector
{
public:
	Projector(Shader* shader, wstring mapFile, UINT width, UINT height);
	~Projector();

	void Update();
	void Render();


private:
	UINT width, height;
	Shader* shader;
	Camera* camera;

};