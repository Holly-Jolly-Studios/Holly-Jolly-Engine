#pragma once
#include "NewTransform.h"
#include "raylib.h"


class RectangleRenderer
{
public:
    RectangleRenderer() = default;

    RectangleRenderer(float width, float height, NewTransform pos, Color color)
    {
        m_Width = width;
        m_Height = height;
        m_CurrentPos = pos;
        m_Color = color;
        m_OriginalColor = color;
    }

    ~RectangleRenderer() = default;


    void RenderRectangle()
    {
        DrawRectangle((int)m_CurrentPos.GetX(), (int)m_CurrentPos.GetY(), (int)m_Width, (int)m_Height, m_Color);
    }

    void UpdatePos(NewTransform newPos)
    {
        m_CurrentPos = newPos;
    }

    void SetColor(Color color)
    {
        m_Color = color;
    }

    void ResetColor()
    {
        m_Color = m_OriginalColor;
    }

protected:
    float m_Width, m_Height;
    NewTransform m_CurrentPos;
    Color m_Color;
    Color m_OriginalColor;

};