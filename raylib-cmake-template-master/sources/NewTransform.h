#pragma once

class NewTransform
{
public:
    NewTransform() = default;

    NewTransform(float xPos, float yPos)
    {
        m_X = xPos;
        m_Y = yPos;
    }

    ~NewTransform() = default;

    float const GetX()
    {
        return m_X;
    }

    float const GetY()
    {
        return m_Y;
    }

    void SetX(float newX)
    {
        m_X = newX;
    }
    void SetY(float newY)
    {
        m_Y = newY;
    }

protected:
    float m_X, m_Y;
};