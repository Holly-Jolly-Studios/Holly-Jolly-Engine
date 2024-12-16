#pragma once

#include "Base/BaseComponent.h"

class NewTransform : public BaseComponent
{

public:
	NewTransform();
	NewTransform(float x, float y);
	~NewTransform();

	float GetX();
	float GetY();

	void SetX(float x);
	void SetY(float y);


private:

	float m_X;
	float m_Y;
};
