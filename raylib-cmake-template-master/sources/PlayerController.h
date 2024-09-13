#pragma once

#include "NewTransform.h"
#include "raylib.h"

class PlayerController
{
public:
    PlayerController() = default;

    PlayerController(float speed, NewTransform pos)
    {
        m_Speed = speed;
        m_CurrentPos = pos;
    }

    ~PlayerController() = default;

    void Update()
    {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        {
            m_CurrentPos.SetX(m_CurrentPos.GetX() - m_Speed);
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        {
            m_CurrentPos.SetX(m_CurrentPos.GetX() + m_Speed);
        }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        {
            m_CurrentPos.SetY(m_CurrentPos.GetY() - m_Speed);
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        {
            m_CurrentPos.SetY(m_CurrentPos.GetY() + m_Speed);
        }
    }

    NewTransform GetTransform()
    {
        return m_CurrentPos;
    }

protected:
    float m_Speed;
    NewTransform m_CurrentPos;
};
