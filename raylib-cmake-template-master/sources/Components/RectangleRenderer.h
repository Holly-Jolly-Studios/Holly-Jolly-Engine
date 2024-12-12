#pragma once

#include "raylib.h"
#include "Base/BaseComponent.h"

class RectangleRenderer : public BaseComponent
{
public:
	RectangleRenderer();
	RectangleRenderer(float width, float height, float x, float y, Color color);
	~RectangleRenderer();

	// Helpers
	float GetWidth();
	float GetHeight();
	float GetTopX();
	float GetTopY();
	Color GetColor();

	void SetWidth(float newWidth);
	void SetHeight(float newHeight);
	void SetTopLeftX(float newX);
	void SetTopLeftY(float newY);
	void SetColor(Color newColor);

	// Change Color
	void SetRValue(float value);
	void SetGValue(float value);
	void SetBValue(float value);
	void SetAValue(float value);

private:

	float m_Width;
	float m_Height;
	float m_TopLeftX;
	float m_TopLeftY;
	Color m_Color;
};