#include "Framework.h"
#include "Projection.h"

Projection::Projection()
	: width(0), height(0), zn(0), zf(0), fov(0)
{
	matrix = XMMatrixIdentity();
}

Projection::Projection(float width, float height, float zn, float zf, float fov)
	: width(width), height(height), zn(zn), zf(zf), fov(fov)
{

}

Projection::~Projection()
{

}

Matrix Projection::GetMatrix()
{
	return matrix;
}

void Projection::Set(float width, float height, float zn, float zf, float fov)
{
	this->width = width;
	this->height = height;
	this->zn = zn;
	this->zf = zf;
	this->fov = fov;
}
