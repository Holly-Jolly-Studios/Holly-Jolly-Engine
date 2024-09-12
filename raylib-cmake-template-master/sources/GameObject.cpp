#include "raylib.h"

class NewTransform;
class RectangleRenderer;
class RectangleCollider;
class PlayerController;
class ColliderColorChanger;

class GameObject
{
public:
    GameObject()
    {
        //transform = { 0, 0 };
        renderer = nullptr;
        collider = nullptr;
        player = nullptr;
        colorChanger = nullptr;
    }

    RectangleRenderer* CreateRenderer(int width, int height, NewTransform pos, Color color) 
    {
        renderer = new RectangleRenderer(width, height, pos, color);
        return renderer;
    }

    RectangleCollider* CreateCollider(int width, int height) 
    {
        collider = new RectangleCollider(width, height);
        return collider;
    }

    PlayerController* CreatePlayerController(float speed, NewTransform pos) 
    {
        player = new PlayerController(speed, pos);
        return player;
    }

    ColliderColorChanger* CreateColliderColorChanger(Color color) 
    {
        colorChanger = new ColliderColorChanger(color);
        return colorChanger;
    }

    void Update() 
    {
        if (player) 
        {
            player->Update();
        }

        // Handle rendering
        if (renderer) 
        {
            renderer->RenderRectangle();
        }
    }

    void CheckCollision(GameObject& other) {
        if (collider && other.collider) {
            bool isColliding = collider->CheckCollision(other.collider, transform);

            // If there's a color changer, update color on collision
            if (colorChanger && renderer) {
                colorChanger->ChangeColorOnCollision(renderer, isColliding);
            }
        }
    }

    ~GameObject() 
    {
        renderer->~RectangleRenderer();
        collider->~RectangleCollider();
        player->~PlayerController();
        colorChanger->~ColliderColorChanger();

        delete renderer;
        delete collider;
        delete player;
        delete colorChanger;
    }

protected:
    NewTransform transform;
    RectangleRenderer* renderer;
    RectangleCollider* collider;
    PlayerController* player;
    ColliderColorChanger* colorChanger;
};


class NewTransform
{
public:
    NewTransform() = default;

    NewTransform(float xPos, float yPos)
    {
        m_X = xPos;
        m_Y = yPos;
    }

    ~NewTransform() = default;

    NewTransform const GetTransform()
    {
        return NewTransform{ m_X, m_Y };
    }

    float const GetX()
    {
        return m_X;
    }
    
    float const GetY()
    {
        return m_Y;
    }

    void SetX(float newX) 
    { 
        m_X = newX;
    }
    void SetY(float newY) 
    { 
        m_Y = newY;
    }

protected:
    float m_X, m_Y;
    friend class GameObject;
};

class RectangleRenderer
{
public:
    RectangleRenderer() = default;

    RectangleRenderer(float width, float height, NewTransform pos, Color color)
    {
        m_Width = width;
        m_Height = height;
        m_CurrentPos = pos;
        m_Color = color;
    }

    ~RectangleRenderer() = default;

    
    void RenderRectangle()
    {
        DrawRectangle(m_CurrentPos.GetX(), m_CurrentPos.GetY(), m_Width, m_Height, m_Color);
    }

    void SetColor(Color color)
    {
        m_Color = color;
    }

protected:
    float m_Width, m_Height;
    NewTransform m_CurrentPos;
    Color m_Color;

    friend class GameObject;
};



//might have errors
class RectangleCollider
{
public:
    RectangleCollider() = default;

    RectangleCollider(float width, float height)
    {
        m_Width = width;
        m_Height = height;
    };

    ~RectangleCollider() = default;


    bool CheckCollision(RectangleCollider* other, NewTransform transform)
    {
        bool collision = false;

        if ((transform.GetX() < (other->m_Transform.GetX() + other->m_Width) && (transform.GetX() + m_Width) > other->m_Transform.GetX()) &&
            (transform.GetY() < (other->m_Transform.GetY() + other->m_Height) && (transform.GetY() + m_Height) > other->m_Transform.GetY())) collision = true;

        return collision;
    }
   
protected:
    float m_Width, m_Height;
    NewTransform m_Transform;

    friend class GameObject;
};


class PlayerController
{
public: 
    PlayerController() = default;

    PlayerController(float speed, NewTransform pos)
    {
        //TODO: Work with delta time
        m_Speed = speed;
        m_CurrentPos = pos;
    }

    ~PlayerController() = default;


    // Updates player pos based on current pos
    void Update()
    {   
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))    m_CurrentPos.SetX(m_CurrentPos.GetX() - m_Speed);
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))   m_CurrentPos.SetX(m_CurrentPos.GetX() + m_Speed);
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))      m_CurrentPos.SetY(m_CurrentPos.GetY() + m_Speed);
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))    m_CurrentPos.SetY(m_CurrentPos.GetY() - m_Speed);
    }

protected:
    float m_Speed;
    NewTransform m_CurrentPos;
};

class ColliderColorChanger
{
public:
    ColliderColorChanger() = default;

    ColliderColorChanger(Color color)
    {
        m_Color = color;
    }

    ~ColliderColorChanger() = default;

    void ChangeColorOnCollision(RectangleRenderer* renderer, bool collisionOccurred) const 
    {
        if (collisionOccurred) 
        {
            renderer->SetColor(m_Color);
        }
    }

protected:
    Color m_Color;

    friend class GameObject;
};