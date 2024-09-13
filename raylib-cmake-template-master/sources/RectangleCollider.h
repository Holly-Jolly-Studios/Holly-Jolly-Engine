#pragma once

#include "raylib.h"
#include "NewTransform.h"


class RectangleCollider
{
public:
    RectangleCollider() = default;

    RectangleCollider(float width, float height, NewTransform pos)
    {
        m_Width = width;
        m_Height = height;
        m_CurrentPos = pos;
    };

    ~RectangleCollider() = default;

    bool CheckCollision(RectangleCollider* other)
    {
        bool collision = false;

        if ((other->m_CurrentPos.GetX() < (this->m_CurrentPos.GetX() + this->m_Width) && (other->m_CurrentPos.GetX() + other->m_Width) > this->m_CurrentPos.GetX()) &&
            (other->m_CurrentPos.GetY() < (this->m_CurrentPos.GetY() + this->m_Height) && (other->m_CurrentPos.GetY() + other->m_Height) > this->m_CurrentPos.GetY())) collision = true;

        return collision;
    }

    void UpdatePos(NewTransform newPos)
    {
        m_CurrentPos = newPos;
    }

protected:
    float m_Width, m_Height;
    NewTransform m_CurrentPos;
};