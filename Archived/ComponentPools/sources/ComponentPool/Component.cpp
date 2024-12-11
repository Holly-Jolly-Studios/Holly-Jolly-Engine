#include "ComponentPool/Component.h"
#include "GameObject/GameObject.h"


//Renderer
ComponentPool<RectangleRenderer> RectangleRenderer::rectangleRendererPool(POOL_SIZE);

RectangleRenderer::RectangleRenderer()
{
    m_Width = 20;
    m_Height = 10;
    m_Color = BLUE;
    m_X = 100;
    m_Y = 100;
}

RectangleRenderer::RectangleRenderer(float width, float height, Color color)
{
    m_Width = width;
    m_Height = height;
    m_Color = color;

    m_X = 100;
    m_Y = 100;
    m_GameObject = NULL;
}

RectangleRenderer::RectangleRenderer(float width, float height, Color color, GameObject* obj)
{
    m_Width = width;
    m_Height = height;
    m_Color = color;
    m_X = obj->GetTransform()->GetX();
    m_Y = obj->GetTransform()->GetY();
    m_GameObject = obj;
}

void RectangleRenderer::Render()
{
    m_X = m_GameObject->GetTransform()->GetX();
    m_X = m_GameObject->GetTransform()->GetY();
    DrawRectangle(m_X, m_Y, m_Width, m_Height, m_Color);
}


ComponentPool<RectangleCollider> RectangleCollider::rectangleColliderPool(POOL_SIZE);

RectangleCollider::RectangleCollider()
{
    m_X = 100;
    m_Y = 100;
    m_Width = 50;
    m_Height = 50;
    m_Self = new GameObject();
}

RectangleCollider::RectangleCollider(float width, float height, GameObject* obj)
{
    m_Self = obj;
    m_X = obj->GetTransform()->GetX();
    m_Y = obj->GetTransform()->GetY();
    this->m_Width = width;
    this->m_Height = height;
}

bool RectangleCollider::CheckCollision(float otherX, float otherY, float otherWidth, float otherHeight)
{

    if ((m_X < otherX + otherWidth) && m_X + m_Width > otherX && (m_Y < otherY + otherHeight) && m_Y + m_Height > otherY)
    {
        return true;
    }
    return false;
}

//Player
ComponentPool<PlayerController> PlayerController::playerControllerPool(POOL_SIZE);

PlayerController::PlayerController()
{
    m_GameObject  = nullptr;
}

PlayerController::PlayerController(GameObject* obj)
{
    this->m_GameObject = obj;
}

void PlayerController::Update(float dt)
{
    if (m_GameObject != nullptr) 
    {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        {
            m_GameObject->GetTransform()->SetX(m_GameObject->GetTransform()->GetX() - m_Movespeed * dt);
        }
        else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        {
            m_GameObject->GetTransform()->SetX(m_GameObject->GetTransform()->GetX() + m_Movespeed * dt);
        }
        else if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        {
            m_GameObject->GetTransform()->SetY(m_GameObject->GetTransform()->GetY() - m_Movespeed * dt);
        }
        else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        {
            m_GameObject->GetTransform()->SetY(m_GameObject->GetTransform()->GetY() + m_Movespeed * dt);
        }
    }

}

//Color Changer
ComponentPool<ColliderColorChanger> ColliderColorChanger::colliderColorChangerPool(POOL_SIZE);

ColliderColorChanger::ColliderColorChanger()
{
    m_OldColor = BLACK;
    m_NewColor = RED;
    m_GameObject = nullptr;
}

ColliderColorChanger::ColliderColorChanger(GameObject* color, Color newColor)
{
    m_GameObject = color;
    m_NewColor = newColor;
    m_OldColor = m_GameObject->GetRender()->GetColor();
}

void ColliderColorChanger::Update(float x, float y, float width, float height)
{
    if (m_GameObject != nullptr) {
        if (m_GameObject->GetCollider()->CheckCollision(x, y, width, height))
        {
            m_GameObject->GetRender()->SetColor(m_NewColor);
        }
        else
        {
            m_GameObject->GetRender()->SetColor(m_OldColor);
        }
    }
}