#pragma once

#include "raylib.h"
#include "Components/CollisionColorChanger.h"
#include "Components/PlayerController.h"
#include "Components/RectangleCollider.h"
#include "Components/RectangleRenderer.h"

#include <vector>

class GameObject
{
public:

	GameObject();
	~GameObject();

	void Init();
	void Cleanup();
	void Update();

	int GetObjectID();
	void SetObjectID(int NewObjectID);

	void SetRenderer(RectangleRenderer* NewRenderer);
	void SetCollider(RectangleCollider* NewCollider);
	void SetPlayerController(PlayerController* NewController);
	void SetColorChanger(CollisionColorChanger* NewColor);
	void SetTransform(NewTransform* NewTransform);

	NewTransform* GetTransform();
	PlayerController* GetPlayerController();
	RectangleRenderer* GetRenderer();
	RectangleCollider* GetCollider();
	CollisionColorChanger* GetCollisionColorChanger();

protected:

	int m_ObjectID;

	NewTransform* m_GOTransform;
	PlayerController* m_GOPlayerController;
	RectangleRenderer* m_GORenderer;
	RectangleCollider* m_GOCollider;
	CollisionColorChanger* m_GOColorChanger;

};
