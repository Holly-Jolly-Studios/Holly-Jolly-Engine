#pragma once

#include <raylib.h>
#include "Base/BaseComponent.h"
#include "NewTransform.h"

class RectangleCollider : public BaseComponent
{
public:
	RectangleCollider();

	bool CheckCollision(Rectangle rect1, Rectangle rect2);
	bool GetIsColliding();

private:
	bool m_IsColliding;
};
