#pragma once

#include "raylib.h"
#include "ComponentPool.h"
#include "Components/CollisionColorChanger.h"
#include "Components/PlayerController.h"
#include "Components/RectangleCollider.h"
#include "Components/RectangleRenderer.h"

#include <vector>
#include <string>

enum ComponentTypes
{
	collisionColorChangerComponent,
	transformComponent,
	playerControllerComponent,
	rectangleColliderComponent,
	rectangleRendererComponent
};

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

	// Add components
	bool HasComponent(ComponentTypes type);
	void SetTransform(NewTransform* NewTransform);
	void SetPlayerController(PlayerController* NewController);
	void SetCollider(RectangleCollider* NewCollider);
	void SetRenderer(RectangleRenderer* NewRenderer);
	void SetColorChanger(CollisionColorChanger* NewColor);

	// Remove Components
	void RemoveFromComponentList(ComponentTypes type);
	void RemoveTransform(ComponentPool<NewTransform> pool, std::vector<NewTransform*> vec);
	void RemovePlayerController(ComponentPool<PlayerController> pool, std::vector<PlayerController*> vec);
	void RemoveCollider(ComponentPool<RectangleCollider> pool, std::vector<RectangleCollider*> vec);
	void RemoveRenderer(ComponentPool<RectangleRenderer> pool, std::vector<RectangleRenderer*> vec);
	void RemoveColorChanger(ComponentPool<CollisionColorChanger> pool, std::vector<CollisionColorChanger*> vec);
	void RemoveAllComponents();

	// Getters
	NewTransform* GetTransform();
	PlayerController* GetPlayerController();
	RectangleRenderer* GetRenderer();
	RectangleCollider* GetCollider();
	CollisionColorChanger* GetCollisionColorChanger();

	// Name
	void SetName(std::string name);

protected:
	std::string m_Name;
	int m_ObjectID;
	std::vector<ComponentTypes> m_ComponentTypes;

	NewTransform* m_GOTransform;
	PlayerController* m_GOPlayerController;
	RectangleRenderer* m_GORenderer;
	RectangleCollider* m_GOCollider;
	CollisionColorChanger* m_GOColorChanger;

	bool m_HasTransformComponent;
	bool m_HasPlayerControllerComponent;
	bool m_HasColliderComponent;
	bool m_HasRendererComponent;
	bool m_HasColorChangerComponent;
};
