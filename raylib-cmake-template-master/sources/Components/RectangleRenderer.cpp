#include "RectangleRenderer.h"

RectangleRenderer::RectangleRenderer()
{
	Init();
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

void RectangleRenderer::Init()
{
	m_Width = 0;
	m_Height = 0;
	m_Color = { 0, 0, 0, 0 };
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