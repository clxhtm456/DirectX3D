#include "Framework.h"
#include "Projector.h"

Projector::Projector(Shader * shader, wstring mapFile, UINT width, UINT height):
	shader(shader),
	width(width),
	height(height)
{
	camera = new Fixity();
	camera->Position(0, 0, -20);

	proejction = new Orthographic((float)width, (float)height);

	camera->RotationDegree()
}

Projector::~Projector()
{
}

void Projector::Update()
{
}

void Projector::Render()
{
}
