#pragma once

#include "raylib.h"
#include "RectangleRenderer.h"

class ColliderColorChanger
{
public:
    ColliderColorChanger() = default;

    ColliderColorChanger(Color color)
    {
        m_Color = color;
    }

    ~ColliderColorChanger() = default;

    void ChangeColorOnCollision(RectangleRenderer* renderer, bool collisionOccurred) const
    {
        if (collisionOccurred)
        {
            renderer->SetColor(m_Color);
        }
    }

    void ChangeColor(Color newColor)
    {
        m_Color = newColor;
    }

protected:
    Color m_Color;

    friend class GameObject;
};
