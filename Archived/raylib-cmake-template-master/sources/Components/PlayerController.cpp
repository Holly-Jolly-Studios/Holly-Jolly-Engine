#include "PlayerController.h"

PlayerController::PlayerController()
{
	// Do nothing
}

PlayerController::~PlayerController()
{
	// Do nothing
}

int PlayerController::Update(NewTransform* transform)
{
	// WASD movement
	if (IsKeyDown(KEY_D))
	{
		transform->SetX(transform->GetX() + BASE_MOVE_SPEED);
		return 0;
	}
	else if (IsKeyDown(KEY_A))
	{
		transform->SetX(transform->GetX() - BASE_MOVE_SPEED);
		return 0;
	}
	else if (IsKeyDown(KEY_W))
	{
		transform->SetY(transform->GetY() - BASE_MOVE_SPEED);
		return 0;
	}
	else if (IsKeyDown(KEY_S))
	{
		transform->SetY(transform->GetY() + BASE_MOVE_SPEED);
		return 0;
	}
	// Arrow movement
	else if (IsKeyDown(KEY_RIGHT))
	{
		transform->SetX(transform->GetX() + BASE_MOVE_SPEED);
		return 0;
	}
	else if (IsKeyDown(KEY_LEFT))
	{
		transform->SetX(transform->GetX() - BASE_MOVE_SPEED);
		return 0;
	}
	else if (IsKeyDown(KEY_UP))
	{
		transform->SetY(transform->GetY() - BASE_MOVE_SPEED);
		return 0;
	}
	else if (IsKeyDown(KEY_DOWN))
	{
		transform->SetY(transform->GetY() + BASE_MOVE_SPEED);
		return 0;
	}
	
}
