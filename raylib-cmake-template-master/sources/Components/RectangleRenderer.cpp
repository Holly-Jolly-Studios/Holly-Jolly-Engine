#include "RectangleRenderer.h"

RectangleRenderer::RectangleRenderer()
{
	m_Width = 0;
	m_Height = 0;
	m_Color = { 0, 0, 0, 0 };
}

RectangleRenderer::RectangleRenderer(float width, float height, float topLeftX, float topLeftY, Color color)
{
	this->m_Width = width;
	this->m_Height = height;
	this->m_TopLeftX = topLeftX;
	this->m_TopLeftY = topLeftY;
	this->m_Color = color;
}

RectangleRenderer::~RectangleRenderer()
{
	// Do nothing
}

#pragma region Helpers

float RectangleRenderer::GetWidth()
{
	return m_Width;
}

float RectangleRenderer::GetHeight()
{
	return m_Height;
}

float RectangleRenderer::GetTopLeftX()
{
	return m_TopLeftX;
}

float RectangleRenderer::GetTopLeftY()
{
	return m_TopLeftY;
}

Color RectangleRenderer::GetColor()
{
	return m_Color;
}

void RectangleRenderer::SetWidth(float newWidth)
{
	this->m_Width = newWidth;
}

void RectangleRenderer::SetHeight(float newHeight)
{
	this->m_Height = newHeight;
}

void RectangleRenderer::SetColor(Color newColor)
{
	this->m_Color = newColor;
}

void RectangleRenderer::SetTopLeftX(float newX)
{
	m_TopLeftX = newX;
}

void RectangleRenderer::SetTopLeftY(float newY)
{
	m_TopLeftY = newY;
}

#pragma endregion



void RectangleRenderer::SetRValue(float value)
{
	this->m_Color.r = value;
}

void RectangleRenderer::SetGValue(float value) 
{
	this->m_Color.g = value;
}

void RectangleRenderer::SetBValue(float value) 
{
	this->m_Color.b = value;
}

void RectangleRenderer::SetAValue(float value) {
	this->m_Color.a = value;
}
