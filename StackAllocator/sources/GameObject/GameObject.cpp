#include "GameObject.h"
#include "ComponentPool/Component.h"

GameObject::GameObject() {
    m_NewTransform = NewTransform(100, 100);
    m_Renderer = nullptr;
    m_Collider = nullptr;
    m_PlayerController = nullptr;
    m_ColorChanger = nullptr;
}

RectangleRenderer* GameObject::CreateRenderer(float width, float height, Color color)
{
    m_Renderer = RectangleRenderer::rectangleRendererPool.New(width, height, color, this);
    return m_Renderer;
}

RectangleCollider* GameObject::CreateCollider(float width, float height)
{
    m_Collider = RectangleCollider::rectangleColliderPool.New(width, height, this);
    return m_Collider;
};

PlayerController* GameObject::CreatePlayerController()
{
    m_PlayerController = PlayerController::playerControllerPool.New(this);
    return m_PlayerController;
};

ColliderColorChanger* GameObject::CreateColliderColorChanger(Color newColor)
{
    m_ColorChanger = ColliderColorChanger::colliderColorChangerPool.New(this, newColor);
    return m_ColorChanger;
};

void GameObject::Update(float deltaTime, float x, float y, float width, float height)
{
    if (m_PlayerController != nullptr)
    {
        m_PlayerController->Update(deltaTime);
    }

    if (m_ColorChanger != nullptr)
    {
        m_ColorChanger->Update(x, y, width, height);
    }
};

void GameObject::Render()
{
    if (m_Renderer != nullptr)
    {
        m_Renderer->Render();
    }
};