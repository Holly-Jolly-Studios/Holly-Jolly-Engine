#include "RectangleCollider.h"

RectangleCollider::RectangleCollider()
{
	m_IsColliding = false;
}

bool RectangleCollider::CheckCollision(Rectangle rect1, Rectangle rect2)
{
	/*
	 if ((m_X < otherX + otherWidth) && m_X + m_Width > otherX && (m_Y < otherY + otherHeight) && m_Y + m_Height > otherY)
    {
        return true;
    }
    return false;
	*/

	// Use raylib's collision checking instead of our own math
	m_IsColliding = CheckCollisionRecs(rect1, rect2);
	return m_IsColliding;
}

bool RectangleCollider::GetIsColliding()
{
	if (m_IsColliding != NULL)
	{
		return m_IsColliding;
	}
	else
	{
		return false;
	}
}

void RectangleCollider::ResetCollision()
{
	m_IsColliding = false;
}



