#pragma once

#include "raylib.h"
#include "Base/BaseComponent.h"

//requires MyTransform, RectangleRenderer, RectangleCollider

class CollisionColorChanger : public BaseComponent
{
public:
	CollisionColorChanger();
	CollisionColorChanger(Color color, Color defaultColor);
	~CollisionColorChanger();

	bool Update(bool isColliding);

	// Helpers
	void SetChangedColor(Color color);
	void SetRValue(float r);
	void SetGValue(float g);
	void SetBValue(float b);
	void SetAValue(float a);
	Color GetNewColor();
	bool GetIsDefaultColor();

	void SetDefaultColor(Color color);
	Color GetDefaultColor();

private:
	Color m_NewColor;
	Color m_DefaultColor;
	bool m_IsDefaultColor;
	bool m_SwitchedColor;
};
