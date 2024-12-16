#pragma once

#include "raylib.h"
#include "NewTransform.h"
#include "Base/BaseComponent.h"

const float BASE_MOVE_SPEED = 2.0f;

class PlayerController : public BaseComponent
{
public:

	PlayerController();
	~PlayerController();

	int Update(NewTransform* transform);
};
