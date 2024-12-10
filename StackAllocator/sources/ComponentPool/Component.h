#pragma once
#include "raylib.h"
#include "ComponentPool/ComponentPool.h"

class GameObject;

const int POOL_SIZE = 20;

class Component
{
public:
    virtual void Render() {};
    virtual void Update() {};
    virtual ~Component() {};
};

class NewTransform : public Component
{
public:
    NewTransform(float x, float y)
    {
        this->m_X = x;
        this->m_Y = y;
    }

    NewTransform()
    {
        m_X = 50;
        m_Y = 50;
    }

    ~NewTransform() {}

    // Helpers
    float GetX() { return m_X; };
    float GetY() { return m_Y; };

    void SetX(float newX) { m_X = newX; };
    void SetY(float newY) { m_Y = newY; };
    void SetNewTransform(float x, float y) { this->m_X = x; this->m_Y = y; };

private:
    float m_X;
    float m_Y;
};



class RectangleRenderer : public Component
{
public:
    static ComponentPool<RectangleRenderer> rectangleRendererPool;
    RectangleRenderer();

    RectangleRenderer(float width, float height, Color color);

    RectangleRenderer(float width, float height, Color color, GameObject* obj);
    
    //helpers
    float GetX() { return m_X; };
    float GetY() { return m_Y; };

    float GetWidth() { return m_Width; };
    void SetWidth(float newWidth) { m_Width = newWidth; };

    float GetHeight() { return m_Height; };
    void SetHeight(float newHeight) { m_Height = newHeight; };

    Color GetColor() { return m_Color; };
    void SetColor(Color color) { m_Color = color; };

    ~RectangleRenderer() {}

    void Render() override;

private:
    float m_Width;
    float m_Height;
    Color m_Color;
    float m_X, m_Y;
    GameObject* m_GameObject;

};

class RectangleCollider : public Component
{
public:
    static ComponentPool<RectangleCollider> rectangleColliderPool;

    RectangleCollider();

    RectangleCollider(float width, float height, GameObject* obj);

    ~RectangleCollider() = default;

    GameObject* GetSelf() { return m_Self; };

    void SetSelf(GameObject* newObj) { m_Self = newObj; };

    bool CheckCollision(float otherX, float otherY, float otherWidth, float otherHeight);


private:
    bool m_Collision = false;
    float m_Width, m_Height, m_X, m_Y;
    GameObject* m_Self;
};


class PlayerController : public Component {
public:
    static ComponentPool<PlayerController> playerControllerPool;

    PlayerController();

    PlayerController(GameObject* obj);

    ~PlayerController() = default;

    void Update(float dt);

private:
    GameObject* m_GameObject;
    int m_Movespeed = 120;
};

class ColliderColorChanger : public Component
{
public:
    static ComponentPool<ColliderColorChanger> colliderColorChangerPool;

    ColliderColorChanger();
    ColliderColorChanger(GameObject* color, Color newColor);

    ~ColliderColorChanger() = default;
    void Update(float x, float y, float width, float height);

private:
    GameObject* m_GameObject;
    Color m_OldColor;
    Color m_NewColor;
};
