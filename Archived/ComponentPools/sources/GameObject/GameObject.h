#pragma once
#include "ComponentPool/Component.h"
#include "raylib.h"

class GameObject
{
public:
    GameObject();

    RectangleRenderer* CreateRenderer(float width, float height, Color color);

    RectangleCollider* CreateCollider(float width, float height);

    PlayerController* CreatePlayerController();

    ColliderColorChanger* CreateColliderColorChanger(Color newColor);

    void Update(float deltaTime, float x, float y, float width, float height);

    void Render();

    // Helpers
    RectangleRenderer* GetRender() { return m_Renderer; };
    RectangleCollider* GetCollider() { return m_Collider; };
    PlayerController* GetPlayer() { return m_PlayerController; };
    ColliderColorChanger* GetChanger() { return m_ColorChanger; };
    NewTransform* GetTransform() { return &m_NewTransform; };

protected:
    NewTransform m_NewTransform;
    RectangleRenderer* m_Renderer;
    RectangleCollider* m_Collider;
    PlayerController* m_PlayerController;
    ColliderColorChanger* m_ColorChanger;
};
