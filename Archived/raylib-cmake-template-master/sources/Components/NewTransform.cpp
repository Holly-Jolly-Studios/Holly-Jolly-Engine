#include "NewTransform.h"

NewTransform::NewTransform()
{
	m_X = 0;
	m_Y = 0;
}

NewTransform::NewTransform(float x, float y)
{
	this->m_X = x;
	this->m_Y = y;
}

NewTransform::~NewTransform()
{
	// Do nothing
}


#pragma region Helpers

float NewTransform::GetX()
{
	return m_X;
}

float NewTransform::GetY()
{
	return m_Y;
}

void NewTransform::SetX(float x)
{
	this->m_X = x;
}

void NewTransform::SetY(float y)
{
	this->m_Y = y;
}


#pragma endregion
