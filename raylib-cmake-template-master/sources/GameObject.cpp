#include "raylib.h"

#include "NewTransform.h"
#include "RectangleRenderer.h"
#include "RectangleCollider.h"
#include "PlayerController.h"
#include "ColliderColorChanger.h"


class GameObject
{
public:
    GameObject()
    {
        transform = NewTransform{ 0, 0 };
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

    RectangleCollider* CreateCollider(int width, int height, NewTransform pos)
    {
        collider = new RectangleCollider(width, height, pos);
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
            transform = player->GetTransform();

            if (renderer)
            {
                renderer->UpdatePos(player->GetTransform());
            }

            if (collider)
            {
                collider->UpdatePos(player->GetTransform());
            }
        }

        if (renderer)
        {
            renderer->RenderRectangle();
        }
    }

    void CheckCollision(GameObject& other) {
        if (collider && other.collider) {
            bool isColliding = collider->CheckCollision(other.collider);

            if (!isColliding)
            {
                renderer->ResetColor();
            }

            // If it has a color changer, update color on collision
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




